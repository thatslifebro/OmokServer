#include "db_processor.h"

void DBProcessor::Init()
{
	packet_sender_.Init([&](uint32_t session_id, std::shared_ptr<char[]> buffer, int length) {
		auto session = GetSession_(session_id);
		session->SendPacket(buffer, length); });

	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqLogin), [&](Packet packet) { return ReqLoginHandler(packet); }));
}

bool DBProcessor::ProcessDB()
{
	auto packet = PopAndGetPacket_();
	
	if (packet.IsValidSize() == false)
	{
		return false;
	}

	auto error_code = packet_handler_map_[packet.GetPacketId()](packet);
	if (error_code != ErrorCode::None)
	{
		std::print("PacketId : {} 처리 중 에러 발생. ErrorCode : {}\n", packet.GetPacketId(), static_cast<int>(error_code));
	}

	return true;
}

ErrorCode DBProcessor::ReqLoginHandler(Packet packet)
{
	OmokPacket::ReqLogin req_login;
	req_login.ParseFromArray(packet.GetPacketBody(), packet.GetBodySize());

	auto session = GetSession_(packet.GetSessionId());
	if (IsValidSession(session) == false)
	{
		return ErrorCode::InvalidSession;
	}

	if (session->IsLoggedIn() == true)
	{
		return ErrorCode::AlreadyLoggedIn;
	}

	session->SetLoggedIn(true);
	session->SetUserId(req_login.userid());

	packet_sender_.ResLogin(session->GetSessionId(), 0);

	std::print("유저 {} 로그인\n", req_login.userid());

	return ErrorCode::None;
}

bool DBProcessor::IsValidSession(Session* session)
{
	if (session == nullptr)
	{
		return false;
	}

	return true;
}