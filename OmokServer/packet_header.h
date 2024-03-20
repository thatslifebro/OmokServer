#pragma once
#include <memory>

class PacketHeader
{
public:
	static const uint16_t HEADER_SIZE = 4;

	uint16_t& GetPacketId() { return packet_id_; }

	uint16_t& GetPacketSize() { return packet_size_; }

	void SetPacketId(uint16_t packet_id) { packet_id_ = packet_id; }

	void SetPacketSize(uint16_t packet_size) { packet_size_ = packet_size; }

private:
	uint16_t packet_id_;
	uint16_t packet_size_;
};
