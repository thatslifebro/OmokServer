#include "ready_handler.h"

void ReadyHandler::Init(std::function<Room* (uint32_t room_id)> GetRoom,
	std::function<Session* (uint32_t session_id)> GetSession)
{
	packet_sender_.InitSendPacketFunc([&](uint32_t session_id, char* buffer, int length) {
		auto session = GetSession_(session_id);
		session->SendPacket(buffer, length); });

	GetRoom_ = GetRoom;
	GetSession_ = GetSession;
}

ErrorCode ReadyHandler::HandleRequest(Packet packet, uint32_t time_count)
{
	auto session = GetSession_(packet.GetSessionId());
	if (session == nullptr)
	{
		return ErrorCode::InvalidSession;
	}

	auto session_id = session->GetSessionId();
	auto room = GetRoom_(session->GetRoomId());
	auto game = room->GetGame();

	auto error_code = CheckError(session_id, session, room, game);
	if (error_code != ErrorCode::None)
	{
		return error_code;
	}

	std::print("UserId : {} 준비 완료.\n", session->GetUserId());

	ProcessRequest(session_id, session, room, game, time_count);

	return ErrorCode::None;
}

void ReadyHandler::ProcessRequest(uint32_t session_id, Session* session, Room* room, Game* game, uint32_t time_count)
{
	game->SetReady(session_id);

	packet_sender_.ResReadyOmok(session_id);

	game->StartGameIfBothReady();

	if (room->IsGameStarted())
	{
		std::print("{}번 Room 게임 시작!\n", session->GetRoomId());

		auto [black_session_id, black_user_id, white_session_id, white_user_id] = room->GetPlayerInfo();

		packet_sender_.NtfStartOmok(black_session_id, black_user_id, white_session_id, white_user_id);

		room->NtfStartOmokView(black_session_id, black_user_id, white_session_id, white_user_id);

		room->SetRepeatedTimer(time_count, PUT_MOK_TIMEOUT, [room]()
			{
				if (room->IsGameStarted())
				{
					auto game = room->GetGame();
					game->ChangeTurn();

					room->NtfPutMokTimeout();
					std::print("PutMok 시간 초과\n");
				}
			});
	}
}

ErrorCode ReadyHandler::CheckError(uint32_t session_id, Session* session, Room* room, Game* game)
{
	if (session->IsInRoom() == false)
	{
		return ErrorCode::NotInRoom;
	}

	if (room->IsPlayer(session_id) == false)
	{
		return ErrorCode::NotPlayer;
	}

	if (room->IsGameStarted() == true)
	{
		return ErrorCode::GameAlreadyStarted;
	}

	if (game->IsReady(session_id))
	{
		return ErrorCode::AlreadyReady;
	}

	return ErrorCode::None;
}