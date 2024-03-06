#pragma once
#include "Poco/Net/StreamSocket.h"


class PacketInfo
{
public:
	static const uint16_t header_size_ = 4;
	static const uint16_t max_packet_size_ = 1024;
};

class Packet
{

public:
	Packet() = default;
	~Packet() {
		//delete[] packet_body_;
	}
	Packet(uint16_t packet_body_size, uint16_t packet_id, char* packet_body, Poco::Net::StreamSocket* socket) : packet_body_size_(packet_body_size), packet_id_(packet_id), socket_(socket)
	{
		packet_body_ = new char[packet_body_size_];
		memcpy(packet_body_, packet_body, packet_body_size_);
	}
	uint16_t packet_body_size_ = 0;
	uint16_t packet_id_ = 0;
	char* packet_body_;
	Poco::Net::StreamSocket* socket_;

};