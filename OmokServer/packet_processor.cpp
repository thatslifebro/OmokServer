#include "packet_processor.h"

void PacketProcessor::Init()
{
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqLogin), [&](Packet packet) { ReqLoginHandler(packet); }));
	//todo: 패킷에 대한 핸들러 등록
}

bool PacketProcessor::ProcessPacket()
{
	PacketQueue packet_queue;
	const auto& packet = packet_queue.PopAndGetPacket();

	if (packet.packet_size_ <= 0)
	{
		return false;
	}

	packet_handler_map_[packet.packet_id_](packet);
	return true;
}

void PacketProcessor::ReqLoginHandler(Packet packet)
{
	OmokPacket::ReqLogin reqLogin;
	reqLogin.ParseFromArray(packet.packet_body_, packet.packet_size_);

	std::print("받은 메시지 : reqLogin.userid = {}, pw = {}\n", reqLogin.userid(), reqLogin.pw());
	
	auto session = GetSession(packet.session_id_);
	if(session == nullptr)
	{
		return;
	}

	//protobuf
	OmokPacket::ResLogin res_login;

	if (user_auth_map[reqLogin.userid()] == reqLogin.pw())
	{
		session->Login();
		res_login.set_result(0);
	}
	else
	{
		res_login.set_result(-1);
	}

	//ResLogin packet
	auto [res_data, res_length] = MakeResData(res_login);

	// 응답
	session->SendPacket(res_data, res_length);
	std::print("ResLogin to SessionId : {}\n",packet.session_id_);

	delete res_data;
}

template <typename T>
std::tuple<char*, uint16_t> PacketProcessor::MakeResData(T packet_body)
{
	Packet res_login_packet;

	res_login_packet.packet_id_ = static_cast<uint16_t>(PacketId::ResLogin);
	res_login_packet.packet_size_ = packet_body.ByteSizeLong() + PacketHeader::header_size_;
	packet_body.SerializeToArray(res_login_packet.packet_body_, packet_body.ByteSizeLong());

	return res_login_packet.ToByteArray();
}

Session* PacketProcessor::GetSession(int session_id)
{
	SessionManager session_manager;
	if (session_manager.IsSessionExist(session_id) == false)
	{
		std::print("Session is not exist\n");
		return nullptr;
	}

	return session_manager.GetSession(session_id);
}
