#include "put_mok_handler.h"

void PutMokHandler::Init(std::function<Room* (uint32_t room_id)> GetRoom,
	std::function<Session* (uint32_t session_id)> GetSession)
{
	packet_sender_.InitSendPacketFunc([&](uint32_t session_id, char* buffer, int length) {
		auto session = GetSession_(session_id);
		session->SendPacket(buffer, length); });

	GetRoom_ = GetRoom;
	GetSession_ = GetSession;
}

ErrorCode PutMokHandler::HandleRequest(Packet packet, uint32_t time_count)
{
	auto [x, y] = ExtractPacketData(packet);

	auto session = GetSession_(packet.GetSessionId());
	if (session == nullptr)
	{
		return ErrorCode::InvalidSession;
	}

	auto session_id = session->GetSessionId();
	auto room = GetRoom_(session->GetRoomId());
	auto game = room->GetGame();

	auto error_code = CheckError(session_id, room, game, x, y);
	if (error_code != ErrorCode::None)
	{
		return error_code;
	}

	std::print("UserId : {} 가 ({}, {})에 돌을 놓음.\n", session->GetUserId(), x, y);

	ProcessRequest(session_id, room, game, x, y, time_count);

	return ErrorCode::None;
}

std::tuple<uint32_t, uint32_t> PutMokHandler::ExtractPacketData(Packet packet)
{
	OmokPacket::ReqPutMok req_put_mok;
	req_put_mok.ParseFromArray(packet.GetPacketBody(), packet.GetBodySize());

	auto x = req_put_mok.x();
	auto y = req_put_mok.y();

	return std::make_tuple(x, y);
}

void PutMokHandler::ProcessRequest(uint32_t session_id, Room* room, Game* game, uint32_t x, uint32_t y, uint32_t time_count)
{
	game->SetStone(x, y, session_id);

	packet_sender_.ResPutMok(session_id, 0);
	room->CancelTimer();

	room->NtfPutMok(session_id, x, y);

	if (game->CheckOmok(x, y) == true)
	{
		auto winner_id = game->WinnerId();
		auto loser_id = game->LoserId();

		packet_sender_.NtfGameOver(winner_id, 1);
		packet_sender_.NtfGameOver(loser_id, 0);

		room->NtfGameOverView(winner_id, loser_id);

		//게임 종료 처리
		room->EndMatch();

		std::print("{}번 방 게임 종료. {} 승리 ,{} 패배\n", GetSession_(session_id)->GetRoomId(), GetSession_(winner_id)->GetUserId(), GetSession_(loser_id)->GetUserId());
	}
	else
	{
		//타이머 다시 설정
		room->SetSameWithPreviousTimer(time_count);
	}
}

ErrorCode PutMokHandler::CheckError(uint32_t session_id, Room* room, Game* game, uint32_t x, uint32_t y)
{
	if (room->IsGameStarted() == false)
	{
		packet_sender_.ResPutMok(session_id, static_cast<uint32_t>(ErrorCode::GameNotStarted));
		return ErrorCode::GameNotStarted;
	}
	if (room->IsPlayer(session_id) == false)
	{
		packet_sender_.ResPutMok(session_id, static_cast<uint32_t>(ErrorCode::NotPlayer));
		return ErrorCode::NotPlayer;
	}
	if (game->CheckTurn(session_id) == false)
	{
		packet_sender_.ResPutMok(session_id, static_cast<uint32_t>(ErrorCode::NotYourTurn));
		return ErrorCode::NotYourTurn;
	}

	if (game->CanSetStone(x, y, session_id) == false)
	{
		packet_sender_.ResPutMok(session_id, static_cast<uint32_t>(ErrorCode::PutMokFail));
		return ErrorCode::PutMokFail;
	}

	return ErrorCode::None;
}