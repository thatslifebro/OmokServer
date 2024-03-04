#pragma once
#include "packet_processor.h"
#include "packet_info.h"
#include "packet_id.h"

PacketProcessor::PacketProcessor() {
	PacketBuffer = PacketBufferManager::GetInstance();
	packetHandlerMap.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqLogin), [&](const Packet& packet) { ReqLoginHandler(packet); }));
	//todo: ��Ŷ�� ���� �ڵ鷯 ���
}

void PacketProcessor::ProcessPacket(char* packet_buffer)
{
	Packet packet;
	packet.packet_size = ((packet_buffer[0] & 0xFF) << 8) | (packet_buffer[1] & 0xFF);
	packet.packet_id = ((packet_buffer[2] & 0xFF) << 8) | (packet_buffer[3] & 0xFF);

    std::cout << "���� ��Ŷ ������ : " << packet.packet_size << ", ��Ŷ ���̵� : " << packet.packet_id << std::endl;
	packet.packet_body = packet_buffer + PacketInfo::header_size_;
	std::cout << "ReqLoginHandler ȣ��" << std::endl;
	packetHandlerMap[packet.packet_id](packet);
}

void PacketProcessor::ReqLoginHandler(const Packet& packet)
{
	
	OmokPacket::ReqLogin reqLogin;
	reqLogin.ParseFromArray(packet.packet_body, packet.packet_size - PacketInfo::header_size_);

	std::cout << "���� �޽��� : reqLogin.userid = " << reqLogin.userid() << ", pw = " << reqLogin.pw() << std::endl;
}
