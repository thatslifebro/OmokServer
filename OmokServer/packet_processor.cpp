#include "packet_processor.h"
#include "packet_info.h"
#include "packet_id.h"

PacketProcessor::PacketProcessor() {
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqLogin), [&](const Packet& packet) { ReqLoginHandler(packet); }));
	//todo: ��Ŷ�� ���� �ڵ鷯 ���
}

void PacketProcessor::ProcessPacket(char* packet_buffer)
{
	Packet packet;
	//little endian
	char packet_size[2] = { packet_buffer[0], packet_buffer[1] };
	char packet_id[2] = { packet_buffer[2], packet_buffer[3] };

	packet.packet_size_ = *reinterpret_cast<uint16_t*>(packet_size);
	packet.packet_id_ = *reinterpret_cast<uint16_t*>(packet_id);
	std::print("���� ��Ŷ ������ : {}, ��Ŷ ���̵� : {}\n", packet.packet_size_, packet.packet_id_);

	packet.packet_body_ = packet_buffer + PacketInfo::header_size_;
	packet_handler_map_[packet.packet_id_](packet);
}

void PacketProcessor::ReqLoginHandler(const Packet& packet)
{
	OmokPacket::ReqLogin reqLogin;
	reqLogin.ParseFromArray(packet.packet_body_, packet.packet_size_ - PacketInfo::header_size_);

	std::print("���� �޽��� : reqLogin.userid = {}, pw = {}\n", reqLogin.userid(), reqLogin.pw());
}
