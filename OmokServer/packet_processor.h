#pragma once
#include <functional>
#include <iostream>
#include <unordered_map>
#include <bit>

#include "protobuf/OmokPacket.pb.h"
#include "packet_queue.h"

class PacketProcessor
{
public:
	PacketProcessor();
	bool ProcessPacket();
	void ReqLoginHandler(Packet pkt);

private:
	std::unordered_map<uint16_t, std::function<void(Packet) >> packet_handler_map_;
	PacketQueue packet_queue_;
};
