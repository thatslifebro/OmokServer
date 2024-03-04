#pragma once
#include <iostream>
#include <unordered_map>
#include <functional>
#include "packet_buffer_manager.h"
#include "protobuf/OmokPacket.pb.h"

struct Packet
{
	uint16_t packet_size;
	uint16_t packet_id;
	char* packet_body;
};

class PacketProcessor
{
public:
	PacketProcessor();
	void ProcessPacket(char* pktBuf);
	void ReqLoginHandler(const Packet& pkt);

private:
	PacketBufferManager* PacketBuffer;
	std::unordered_map<uint16_t, std::function<void(const Packet&) >> packetHandlerMap;
};
