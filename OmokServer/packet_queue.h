#pragma once
#include<mutex>
#include<queue>
#include<print>

#include "Poco/Net/StreamSocket.h"

#include "packet_info.h"

// todo : static 안써보기
class PacketQueue
{
public:
	void Save(std::shared_ptr<char[]> buffer, uint32_t length, uint32_t session_id);

	Packet PopAndGetPacket();
	
private:
	std::queue<Packet> packet_queue_;
};

class DBPacketQueue
{
public:
	void PushPacket(const Packet& packet);

	const Packet& PopAndGetPacket();

private:
	static std::mutex db_mutex_;
	static std::queue<Packet> db_packet_queue_;
};