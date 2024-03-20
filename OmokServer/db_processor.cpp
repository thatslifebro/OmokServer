#include "db_processor.h"

void DBProcessor::Init()
{
	packet_sender_.InitSendPacketFunc([&](uint32_t session_id, char* buffer, int length) {
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
	auto user_id = ReqLoginPacketData(packet);

	auto session = GetSession_(packet.GetSessionId());
	if (IsValidSession(session) == false)
	{
		return ErrorCode::InvalidSession;
	}

	if (session->IsLoggedIn() == true)
	{
		return ErrorCode::AlreadyLoggedIn;
	}

	std::print("유저 {} 로그인\n", user_id);

	ReqLoginProcess(session, user_id);

	return ErrorCode::None;
}

std::string DBProcessor::ReqLoginPacketData(Packet packet)
{
	OmokPacket::ReqLogin req_login;
	req_login.ParseFromArray(packet.GetPacketBody(), packet.GetBodySize());

	auto user_id = req_login.userid();

	return user_id;
}

void DBProcessor::ReqLoginProcess(Session* session, std::string user_id)
{
	session->SetLoggedIn(true);
	session->SetUserId(user_id);

	packet_sender_.ResLogin(session->GetSessionId(), 0);
}

bool DBProcessor::IsValidSession(Session* session)
{
	if (session == nullptr)
	{
		return false;
	}

	return true;
}