#pragma once
#include<mutex>
#include<queue>
#include<print>
#include<concurrent_queue.h>

#include "packet_info.h"
#include "concurrent_queue.h"

class PacketQueue
{
public:
	void SaveByteArray(std::shared_ptr<char[]> buffer, uint32_t length, uint32_t session_id);

	void SavePacket(Packet packet);

	Packet PopAndGetPacket();
	
private:
	Concurrency::concurrent_queue<Packet> packet_queue_;
};
