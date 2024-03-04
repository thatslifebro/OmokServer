#pragma once
#include "packet_processor.h"
#include "packet_info.h"
#include "packet_id.h"

PacketProcessor::PacketProcessor() {
	packet_buffer_ = PacketBufferManager::GetInstance();
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqLogin), [&](const Packet& packet) { ReqLoginHandler(packet); }));
	//todo: 패킷에 대한 핸들러 등록
}

void PacketProcessor::ProcessPacket(char* packet_buffer)
{
	Packet packet;
	//little endian
	packet.packet_size_ = ((packet_buffer[1] & 0xFF) << 8) | (packet_buffer[0] & 0xFF);
	packet.packet_id_ = ((packet_buffer[3] & 0xFF) << 8) | (packet_buffer[2] & 0xFF);

    std::cout << "받은 패킷 사이즈 : " << packet.packet_size_ << ", 패킷 아이디 : " << packet.packet_id_ << std::endl;
	packet.packet_body_ = packet_buffer + PacketInfo::header_size_;
	std::cout << "ReqLoginHandler 호출" << std::endl;
	packet_handler_map_[packet.packet_id_](packet);
}

void PacketProcessor::ReqLoginHandler(const Packet& packet)
{
	OmokPacket::ReqLogin reqLogin;
	reqLogin.ParseFromArray(packet.packet_body_, packet.packet_size_ - PacketInfo::header_size_);

	std::cout << "받은 메시지 : reqLogin.userid = " << reqLogin.userid() << ", pw = " << reqLogin.pw() << std::endl;
}
