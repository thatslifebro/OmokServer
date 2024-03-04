#pragma once
#include <functional>
#include <iostream>
#include <unordered_map>

#include "protobuf/OmokPacket.pb.h"

#include "packet_buffer_manager.h"

struct Packet
{
	uint16_t packet_size_;
	uint16_t packet_id_;
	char* packet_body_;
};

class PacketProcessor
{
public:
	PacketProcessor();
	void ProcessPacket(char* pktBuf);
	void ReqLoginHandler(const Packet& pkt);

private:
	PacketBufferManager* packet_buffer_;
	std::unordered_map<uint16_t, std::function<void(const Packet&) >> packet_handler_map_;
};
