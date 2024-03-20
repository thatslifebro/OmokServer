#pragma once
#include<concurrent_queue.h>

#include "packet_info.h"
#include "concurrent_queue.h"

class DBPacketQueue
{
public:
	void PushPacket(Packet packet) { db_packet_queue_.push(packet); }

	Packet PopAndGetPacket();

private:
	Concurrency::concurrent_queue<Packet> db_packet_queue_;
};