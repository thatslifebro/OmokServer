#pragma once
#include "Poco/Net/StreamSocket.h"

#include "constants.h"

struct PacketHeader
{
	static const uint16_t header_size_ = 4;

	uint16_t packet_id_;
	uint16_t packet_size_;

	PacketHeader(uint16_t packet_id, uint16_t packet_size_): packet_id_(packet_id), packet_size_(packet_size_) {}

	std::shared_ptr<char[]> HeaderToByteArray();
};

class Packet
{
public:
	uint16_t packet_size_ = 0;
	uint16_t packet_id_ = 0;
	char packet_body_[MAX_PACKET_SIZE] = { 0, };
	uint32_t session_id_;

	Packet() = default;

	std::tuple<std::shared_ptr<char[]>, uint16_t> ToByteArray();

	void FromByteArray(std::shared_ptr<char[]> buffer);
};