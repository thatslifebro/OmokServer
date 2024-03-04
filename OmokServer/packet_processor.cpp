#pragma once
#include "packet_processor.h"
#include "packet_info.h"
#include "packet_id.h"

PacketProcessor::PacketProcessor() {
	PacketBuffer = PacketBufferManager::GetInstance();
	packetHandlerMap.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqLogin), [&](const Packet& packet) { ReqLoginHandler(packet); }));
	//todo: 패킷에 대한 핸들러 등록
}

void PacketProcessor::ProcessPacket(char* packet_buffer)
{
	Packet packet;
	packet.packet_size = ((packet_buffer[0] & 0xFF) << 8) | (packet_buffer[1] & 0xFF);
	packet.packet_id = ((packet_buffer[2] & 0xFF) << 8) | (packet_buffer[3] & 0xFF);

    std::cout << "받은 패킷 사이즈 : " << packet.packet_size << ", 패킷 아이디 : " << packet.packet_id << std::endl;
	packet.packet_body = packet_buffer + PacketInfo::header_size_;
	std::cout << "ReqLoginHandler 호출" << std::endl;
	packetHandlerMap[packet.packet_id](packet);
}

void PacketProcessor::ReqLoginHandler(const Packet& packet)
{
	
	OmokPacket::ReqLogin reqLogin;
	reqLogin.ParseFromArray(packet.packet_body, packet.packet_size - PacketInfo::header_size_);

	std::cout << "받은 메시지 : reqLogin.userid = " << reqLogin.userid() << ", pw = " << reqLogin.pw() << std::endl;
}
