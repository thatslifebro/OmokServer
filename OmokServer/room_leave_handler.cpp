#include "room_leave_handler.h"

void RoomLeaveHandler::Init(std::function<void(uint32_t session_id, uint32_t room_id)> UserLeaveRoom,
	std::function<Room* (uint32_t room_id)> GetRoom,
	std::function<Session* (uint32_t session_id)> GetSession)
{
	packet_sender_.InitSendPacketFunc([&](uint32_t session_id, char* buffer, int length) {
		auto session = GetSession_(session_id);
		session->SendPacket(buffer, length); });

	UserLeaveRoom_ = UserLeaveRoom;
	GetRoom_ = GetRoom;
	GetSession_ = GetSession;
}

ErrorCode RoomLeaveHandler::HandleRequest(Packet packet)
{
	auto session = GetSession_(packet.GetSessionId());
	if (session == nullptr)
	{
		return ErrorCode::InvalidSession;
	}

	auto session_id = session->GetSessionId();
	auto room_id = session->GetRoomId();
	auto room = GetRoom_(room_id);

	auto error_code = CheckError(session_id, session);
	if (error_code != ErrorCode::None)
	{
		return error_code;
	}

	std::print("UserId : {} 가 방에서 나감.\n", session->GetUserId());

	ProcessRequest(session_id, session, room_id, room);

	return ErrorCode::None;
}

void RoomLeaveHandler::ProcessRequest(uint32_t session_id, Session* session, uint32_t room_id, Room* room)
{
	UserLeaveRoom_(session_id, room_id);
	session->SetRoomId(0);

	packet_sender_.ResRoomLeave(session_id, 0);
	room->NtfRoomUserLeave(session_id);

	if (room->IsGameStarted() && room->IsPlayer(session_id))
	{
		packet_sender_.NtfGameOver(session_id, 0);
		NotifyOthersEndGame(room, room_id, session_id);
	}

	if (room->IsMatched() && room->IsPlayer(session_id))
	{
		room->EndMatch();
	}

	if (room->IsAdmin(session_id))
	{
		ProcessChangeAdmin(room);
	}
}

ErrorCode RoomLeaveHandler::CheckError(uint32_t session_id, Session* session)
{
	if (session->IsLoggedIn() == false)
	{
		packet_sender_.ResRoomLeave(session_id, static_cast<int>(ErrorCode::NotLoggedIn));
		return ErrorCode::NotLoggedIn;
	}

	if (session->IsInRoom() == false)
	{
		packet_sender_.ResRoomLeave(session_id, static_cast<int>(ErrorCode::NotInRoom));
		return ErrorCode::NotInRoom;
	}

	return ErrorCode::None;
}

void RoomLeaveHandler::NotifyOthersEndGame(Room* room, uint32_t room_id, uint32_t session_id)
{
	auto opponent_id = room->GetOpponentPlayer(session_id);
	packet_sender_.NtfGameOver(opponent_id, 1);

	room->NtfGameOverView(opponent_id, session_id);

	std::print("{}번 방 게임 종료. {} 승리 ,{} 패배\n", room_id, GetSession_(opponent_id)->GetUserId(), GetSession_(session_id)->GetUserId());
}

void RoomLeaveHandler::ProcessChangeAdmin(Room* room)
{
	room->ChangeAdmin();

	if (room->IsEmpty() == false)
	{
		auto new_admin_id = room->GetAdminId();
		packet_sender_.ResYouAreRoomAdmin(new_admin_id);

		room->NtfNewRoomAdmin();
	}
}