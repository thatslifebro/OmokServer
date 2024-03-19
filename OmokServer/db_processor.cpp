#include "db_processor.h"

void DBProcessor::Init()
{
	packet_sender_.Init([&](uint32_t session_id, std::shared_ptr<char[]> buffer, int length) {
		auto session = GetSession_(session_id);
		session->SendPacket(buffer, length); });

	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqLogin), [&](Packet packet) { ReqLoginHandler(packet); }));
}

bool DBProcessor::ProcessDB()
{
	auto packet = PopAndGetPacket_();
	
	if (packet.IsValidSize() == false)
	{
		return false;
	}

	packet_handler_map_[packet.GetPacketId()](packet);
	return true;
}

void DBProcessor::ReqLoginHandler(Packet packet)
{
	OmokPacket::ReqLogin req_login;
	req_login.ParseFromArray(packet.GetPacketBody(), packet.GetBodySize());

	auto session = GetSession_(packet.GetSessionId());
	if (session == nullptr)
	{
		return;
	}

	if (session->IsLoggedIn() == true)
	{
		return;
	}
	
	// 로그인 처리
	uint32_t result = 0;

	session->SetLoggedIn(true);
	session->SetUserId(req_login.userid());

	packet_sender_.ResLogin(session->GetSessionId(), result);

	std::print("유저 {} 로그인\n", req_login.userid());
}

