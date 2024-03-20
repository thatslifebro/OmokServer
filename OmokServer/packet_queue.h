#pragma once
#include<mutex>
#include<queue>
#include<print>

#include "Poco/Net/StreamSocket.h"

#include "packet_info.h"

class PacketQueue
{
public:
	void SaveByteArray(std::shared_ptr<char[]> buffer, uint32_t length, uint32_t session_id);

	void SavePacket(Packet packet);

	Packet PopAndGetPacket();
	
private:
	std::queue<Packet> packet_queue_;
};

class DBPacketQueue
{
public:
	void PushPacket(Packet packet){ db_packet_queue_.push(packet); }

	Packet PopAndGetPacket();

private:
	std::queue<Packet> db_packet_queue_;
};