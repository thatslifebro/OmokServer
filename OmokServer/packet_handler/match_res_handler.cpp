#include "match_res_handler.h"

void MatchResHandler::Init(std::function<Room* (uint32_t room_id)> GetRoom,
	std::function<Session* (uint32_t session_id)> GetSession)
{
	packet_sender_.InitSendPacketFunc([&](uint32_t session_id, char* buffer, int length) {
		auto session = GetSession_(session_id);
		session->SendPacket(buffer, length); });

	GetRoom_ = GetRoom;
	GetSession_ = GetSession;
}

ErrorCode MatchResHandler::HandleRequest(Packet packet, uint32_t time_count)
{
	//방장의 요청에 대한 응답
	auto accept = ExtractPacketData(packet);

	auto session = GetSession_(packet.GetSessionId());
	if (session == nullptr)
	{
		return ErrorCode::InvalidSession;
	}

	auto session_id = session->GetSessionId();
	auto room = GetRoom_(session->GetRoomId());
	auto admin_id = room->GetAdminId();

	auto error_code = CheckError(session_id, admin_id, room);
	if (error_code != ErrorCode::None)
	{
		return error_code;
	}

	ProcessRequest(session_id, session, admin_id, accept, room, time_count);

	return ErrorCode::None;
}

bool MatchResHandler::ExtractPacketData(Packet packet)
{
	OmokPacket::ReqMatchRes req_match_res;
	req_match_res.ParseFromArray(packet.GetPacketBody(), packet.GetBodySize());

	auto accept = req_match_res.accept();

	return accept;
}

void MatchResHandler::ProcessRequest(uint32_t session_id, Session* session, uint32_t admin_id, bool accept, Room* room, uint32_t time_count)
{
	if (accept == false)
	{
		room->CancelMatch();

		packet_sender_.ResMatch(admin_id, 1);

		room->CancelTimer();

		std::print("매칭 거절 : {} -> {}\n", session->GetUserId(), GetSession_(admin_id)->GetUserId());
	}
	else
	{
		packet_sender_.ResMatch(admin_id, 0);

		packet_sender_.ResMatch(session_id, 0);

		room->MatchComplete(admin_id, session_id);

		room->SetTimer(time_count, READY_TIMEOUT, [&]()
			{
				if (room->IsGameStarted() == false)
				{
					room->EndMatch();
					packet_sender_.NtfReadyTimeout(session_id);
					packet_sender_.NtfReadyTimeout(admin_id);
					std::print("{}번 방 준비 완료 시간 초과\n", session->GetRoomId());
				}
			});

		std::print("매칭 수락 : {} -> {}\n", session->GetUserId(), GetSession_(admin_id)->GetUserId());
	}
}

ErrorCode MatchResHandler::CheckError(uint32_t session_id, uint32_t admin_id, Room* room)
{
	if (room->IsTryMatchingWith(session_id) == false)
	{
		packet_sender_.ResMatch(admin_id, static_cast<uint32_t>(ErrorCode::AlreadyTryMatching));
		return ErrorCode::AlreadyTryMatching;
	}

	return ErrorCode::None;
}