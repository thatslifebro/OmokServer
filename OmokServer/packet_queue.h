#pragma once
#include<mutex>
#include<queue>

#include "Poco/Net/StreamSocket.h"

#include "packet_info.h"

class PacketQueue
{
public:
	void Save(std::shared_ptr<char[]> buffer, uint32_t length, uint32_t session_id);

	const Packet& PopAndGetPacket();
	
private:
	static std::mutex mutex_;
	static std::queue<Packet> packet_queue_;
};

