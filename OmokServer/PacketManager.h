#pragma once
#include "PacketBufferManager.h"
#include "OmokPacket.pb.h"
#include <iostream>
#include <unordered_map>
#include <functional>

struct Packet
{
	unsigned short packetSize;
	unsigned short packetID;
	char* packetBody;
};

class PacketManager
{
private:
	PacketBufferManager* PacketBuffer;
	std::unordered_map<unsigned short, std::function<void(const Packet&)>> packetHandlerMap;

public:
	PacketManager();
	void ProcessPacket(char* pktBuf);
	void ReqLoginHandler(const Packet& pkt);
};
