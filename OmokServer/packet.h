#pragma once
#include "Poco/Net/StreamSocket.h"

#include "constants.h"
#include "packet_header.h"

class Packet
{
public:
	void ParseData(std::shared_ptr<char[]> buffer);

	bool IsValidSize() { return packet_header_.GetPacketSize() >= PacketHeader::HEADER_SIZE && packet_header_.GetPacketSize() <= MAX_PACKET_SIZE; }

	uint16_t GetPacketSize() { return packet_header_.GetPacketSize(); }
	void SetPacketSize(uint16_t packet_size);

	uint16_t GetPacketId() { return packet_header_.GetPacketId(); }
	void SetPacketId(uint16_t packet_id);

	char* GetPacketBody() { return packet_byte_array_ + PacketHeader::HEADER_SIZE; }
	uint16_t GetBodySize() { return packet_header_.GetPacketSize() - PacketHeader::HEADER_SIZE; }

	uint32_t GetSessionId() { return session_id_; }
	void SetSessionId(uint32_t session_id) { session_id_ = session_id; }

	char* GetPacketByteArray() { return packet_byte_array_; }

private:
	PacketHeader packet_header_;
	uint32_t session_id_;
	char packet_byte_array_[MAX_PACKET_SIZE] = { 0, };
};