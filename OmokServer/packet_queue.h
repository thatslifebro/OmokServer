#pragma once
#include<iostream>
#include<mutex>
#include<string>
#include<print>
#include<queue>

#include "Poco/Net/StreamSocket.h"

#include "packet_info.h"

class PacketQueue
{
public:
	void Save(char* buffer, uint32_t session_id);
	const Packet& PopAndGetPacket();
	
private:
	static std::mutex mutex_;
	static std::queue<Packet> packet_queue_;
};

