#include "db_processor.h"

void DBProcessor::Init()
{
	packet_sender_.SendPacket = [&](uint32_t session_id, std::shared_ptr<char[]> buffer, int length) {
		auto session = session_manager_.GetSession(session_id);
		if (session == nullptr)
		{
			return;
		}
		session->SendPacket(buffer, length); };

	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqLogin), [&](Packet packet) { ReqLoginHandler(packet); }));
}

bool DBProcessor::ProcessDB()
{
	auto packet = db_packet_queue_.PopAndGetPacket();
	
	if (packet.packet_size_ <= 0)
	{
		return false;
	}

	packet_handler_map_[packet.packet_id_](packet);
	return true;
}

void DBProcessor::ReqLoginHandler(Packet packet)
{
	OmokPacket::ReqLogin req_login;
	req_login.ParseFromArray(packet.packet_body_, packet.packet_size_ - PacketHeader::header_size_);

	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		return;
	}

	if (session->is_logged_in_ == true)
	{
		return;
	}
	
	// 로그인 처리
	uint32_t result = 1;

	session->is_logged_in_ = true;
	session->user_id_ = req_login.userid();

	packet_sender_.ResLogin(session->session_id_, result);
}

