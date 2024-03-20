#pragma once
#include "Poco/Net/StreamSocket.h"

#include "constants.h"
#include "packet_header.h"

class Packet
{
public:
	Packet() = default;

	std::tuple<char*, uint16_t> GetByteArray();

	void ParseData(std::shared_ptr<char[]> buffer);

	bool IsValidSize() { return packet_header_.GetPacketSize() >= PacketHeader::HEADER_SIZE && packet_header_.GetPacketSize() <= MAX_PACKET_SIZE; }

	uint16_t GetPacketId() { return packet_header_.GetPacketId(); }

	char* GetPacketBody() { return byte_array_ + PacketHeader::HEADER_SIZE; }

	uint16_t GetBodySize() { return packet_header_.GetPacketSize() - PacketHeader::HEADER_SIZE; }

	uint32_t GetSessionId() { return session_id_; }

	void SetSessionId(uint32_t session_id) { session_id_ = session_id; }

	void SetPacketSize(uint16_t packet_size);

	void SetPacketId(uint16_t packet_id);

private:
	PacketHeader packet_header_;
	char byte_array_[MAX_PACKET_SIZE] = { 0, };

	uint32_t session_id_;
};