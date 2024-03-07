#pragma once
#include "Poco/Net/StreamSocket.h"

static constexpr uint16_t MAX_PACKET_SIZE = 1024;

struct PacketHeader
{
	static const uint16_t header_size_ = 4;
	uint16_t packet_id_;
	uint16_t packet_size_;

	PacketHeader(uint16_t packet_id, uint16_t packet_size_): packet_id_(packet_id), packet_size_(packet_size_) {}

	char* HeaderToByteArray()
	{
		char* buffer = new char[PacketHeader::header_size_];
		buffer[0] = packet_size_ & 0xFF;
		buffer[1] = (packet_size_ >> 8) & 0xFF;
		buffer[2] = packet_id_ & 0xFF;
		buffer[3] = (packet_id_ >> 8) & 0xFF;

		return buffer;
	}
};

class Packet
{
public:
	uint16_t packet_size_ = 0;
	uint16_t packet_id_ = 0;
	char packet_body_[MAX_PACKET_SIZE] = { 0, };
	uint32_t session_id_;

	Packet() = default;

	auto ToByteArray()
	{
		PacketHeader header(packet_id_, packet_size_);
		auto header_data = header.HeaderToByteArray();

		char* buffer = new char[packet_size_];
		memcpy(buffer, header_data, PacketHeader::header_size_);
		memcpy(buffer + PacketHeader::header_size_, packet_body_, packet_size_ - PacketHeader::header_size_);

		delete header_data;
		return std::make_tuple(buffer, packet_size_);
	}

	void FromByteArray(char* buffer)
	{
		char size[2]={ buffer[0],buffer[1] };
		packet_size_ = *reinterpret_cast<uint16_t*>(size);
		char id[2] = { buffer[2],buffer[3] };
		packet_id_ = *reinterpret_cast<uint16_t*>(id);
		memcpy(packet_body_, buffer + PacketHeader::header_size_, packet_size_- PacketHeader::header_size_);
	}
};