#pragma once
#include<mutex>
#include<queue>
#include<print>

#include "Poco/Net/StreamSocket.h"

#include "packet_info.h"

class PacketQueue
{
public:
	void Save(std::shared_ptr<char[]> buffer, uint32_t length, uint32_t session_id);

	const Packet& PopAndGetPacket();
	
private:
	std::queue<Packet> packet_queue_;
};

class DBPacketQueue
{
public:
	void PushPacket(const Packet& packet){ db_packet_queue_.push(packet); }

	const Packet& PopAndGetPacket();

private:
	std::queue<Packet> db_packet_queue_;
};