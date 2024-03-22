#include "match_req_handler.h"

void MatchReqHandler::Init(std::function<Room* (uint32_t room_id)> GetRoom,
	std::function<Session* (uint32_t session_id)> GetSession)
{
	packet_sender_.InitSendPacketFunc([&](uint32_t session_id, char* buffer, int length) {
		auto session = GetSession_(session_id);
		session->SendPacket(buffer, length); });

	GetRoom_ = GetRoom;
	GetSession_ = GetSession;
}

ErrorCode MatchReqHandler::HandleRequest(Packet packet, uint32_t time_count)
{
	// 방장의 매칭 요청
	auto opponent_id = ExtractPacketData(packet);

	auto session = GetSession_(packet.GetSessionId());
	if (session == nullptr)
	{
		return ErrorCode::InvalidSession;
	}

	auto session_id = session->GetSessionId();
	auto opponent_session = GetSession_(opponent_id);
	auto room = GetRoom_(session->GetRoomId());

	auto error_code = CheckError(session_id, session, opponent_id, opponent_session, room);
	if (error_code != ErrorCode::None)
	{
		return error_code;
	}

	std::print("방장 {} 이 매칭 요청을 {} 에게 보냄.\n", session->GetUserId(), opponent_session->GetUserId());

	ProcessRequest(session_id, session, opponent_id, opponent_session, room, time_count);

	return ErrorCode::None;
}

uint32_t MatchReqHandler::ExtractPacketData(Packet packet)
{
	OmokPacket::ReqMatch req_match;
	req_match.ParseFromArray(packet.GetPacketBody(), packet.GetBodySize());

	auto opponent_id = req_match.sessionid();

	return opponent_id;
}

void MatchReqHandler::ProcessRequest(uint32_t session_id, Session* session, uint32_t opponent_id, Session* opponent_session, Room* room, uint32_t time_count)
{
	room->TryMatchingWith(opponent_id);

	packet_sender_.NtfMatchReq(opponent_id, session_id, session->GetUserId());

	room->SetTimer(time_count, MATCH_RES_TIMEOUT, [&]()
		{
			if (room->IsTryMatchingWith(opponent_id))
			{
				room->CancelMatch();
				packet_sender_.ResMatch(session_id, static_cast<uint32_t>(ErrorCode::TimeOut));
				packet_sender_.NtfMatchTimeout(opponent_id);
				std::print("매칭 요청 시간 초과 : {} -> {}\n", session->GetUserId(), opponent_session->GetUserId());
			}
		});
}

ErrorCode MatchReqHandler::CheckError(uint32_t session_id, Session* session, uint32_t opponent_id, Session* opponent_session, Room* room)
{
	if (opponent_session == nullptr)
	{
		return ErrorCode::InvalidSession;
	}

	if (session->IsLoggedIn() == false)
	{
		return ErrorCode::NotLoggedIn;
	}

	if (session->IsInRoom() == false)
	{
		return ErrorCode::NotInRoom;
	}

	if (room->IsTryMatching())
	{
		return ErrorCode::AlreadyTryMatching;
	}

	if (room->IsAdmin(session_id) == false)
	{
		return ErrorCode::NotRoomAdmin;
	}

	if (session_id == opponent_id)
	{
		return ErrorCode::SameSessionIdWithOpponent;
	}

	return ErrorCode::None;
}