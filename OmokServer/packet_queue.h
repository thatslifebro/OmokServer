#pragma once
#include<concurrent_queue.h>

#include "packet.h"
#include "concurrent_queue.h"

class PacketQueue
{
public:
	void PushPacketFromData(std::shared_ptr<char[]> buffer, uint32_t length, uint32_t session_id);

	void PushPacket(Packet packet);

	Packet PopAndGetPacket();
	
private:
	Concurrency::concurrent_queue<Packet> packet_queue_;
};
