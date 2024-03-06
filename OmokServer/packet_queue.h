#pragma once
#include<iostream>
#include<mutex>
#include<string>
#include<print>
#include<queue>

#include "Poco/Net/StreamSocket.h"

#include "packet_info.h"

//todo : 이름 변경하기 receive buffer?
class PacketQueue
{
public:
	void Save(Packet packet)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		packet_queue_.push(packet);
	}
	Packet PopAndGetPacket()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (packet_queue_.empty())
		{
			return Packet();
		}
		Packet packet = packet_queue_.front();
		packet_queue_.pop();
		
		return packet;
	}
	
private:
	
	static std::mutex mutex_;
	static std::queue<Packet> packet_queue_;
};

