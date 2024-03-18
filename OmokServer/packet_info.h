#pragma once
#include "Poco/Net/StreamSocket.h"

#include "constants.h"
#include "packet_header.h"

class Packet
{
public:
	Packet() = default;

	std::tuple<std::shared_ptr<char[]>, uint16_t> ToByteArray();

	void FromByteArray(std::shared_ptr<char[]> buffer);

	bool IsValidSize() { return packet_size_ >= PacketHeader::header_size_ && packet_size_ <= MAX_PACKET_SIZE; }

	uint16_t GetPacketId() { return packet_id_; }

	char* GetPacketBody() { return packet_body_; }

	uint16_t GetBodySize() { return packet_size_ - PacketHeader::header_size_; }

	uint32_t GetSessionId() { return session_id_; }

	void SetSessionId(uint32_t session_id) { session_id_ = session_id; }

	void SetPacketSize(uint16_t packet_size) { packet_size_ = packet_size; }

	void SetPacketId(uint16_t packet_id) { packet_id_ = packet_id; }

private:
	uint16_t packet_size_ = 0;
	uint16_t packet_id_ = 0;
	char packet_body_[MAX_PACKET_SIZE] = { 0, };
	uint32_t session_id_;
};