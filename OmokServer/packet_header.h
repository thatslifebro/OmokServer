#pragma once
#include <memory>

struct PacketHeader
{
	static const uint16_t header_size_ = 4;

	uint16_t packet_id_;
	uint16_t packet_size_;

	PacketHeader(uint16_t packet_id, uint16_t packet_size_) : packet_id_(packet_id), packet_size_(packet_size_) {}

	std::shared_ptr<char[]> HeaderToByteArray();
};
