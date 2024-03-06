#include "packet_processor.h"

void PacketProcessor::Init()
{
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqLogin), [&](Packet packet) { ReqLoginHandler(packet); }));
	//todo: ��Ŷ�� ���� �ڵ鷯 ���
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

	std::print("���� �޽��� : reqLogin.userid = {}, pw = {}\n", reqLogin.userid(), reqLogin.pw());
	//todo : �α��� ó�� (�α��� ��Ͽ� ���� id �ֱ�)

	//protobuf
	OmokPacket::ResLogin res_login;
	res_login.set_result(0);

	//ResLogin packet
	auto [res_data, res_length] = MakeResData(res_login);

	// ����
	SendResData(res_data, res_length, packet.session_id_);

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

void PacketProcessor::SendResData(char* res_data, int res_length, int session_id)
{
	SessionManager session_manager;
	if (session_manager.IsSessionExist(session_id) == false)
	{
		std::print("Session is not exist\n");
		return;
	}

	auto session = session_manager.GetSession(session_id);
	session->SendPacket(res_data, res_length);

	std::print("Send to SessionId : {}\n", session_id);
}