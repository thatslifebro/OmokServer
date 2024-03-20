#include "packet.h"

std::tuple<char*, uint16_t> Packet::GetByteArray()
{
	return std::make_tuple(byte_array_, packet_header_.GetPacketSize());
}

void Packet::ParseData(std::shared_ptr<char[]> buffer)
{
	memcpy(&packet_header_.GetPacketSize(), buffer.get(), sizeof(packet_header_.GetPacketSize()));

	memcpy(&packet_header_.GetPacketId(), buffer.get() + sizeof(packet_header_.GetPacketSize()), sizeof(packet_header_.GetPacketId()));

	memcpy(byte_array_, buffer.get(), packet_header_.GetPacketSize());
}

void Packet::SetPacketSize(uint16_t packet_size)
{
	packet_header_.GetPacketSize() = packet_size;
	memcpy(byte_array_, &packet_header_.GetPacketSize(), sizeof(packet_header_.GetPacketSize()));
}

void Packet::SetPacketId(uint16_t packet_id)
{
	packet_header_.GetPacketId() = packet_id;
	memcpy(byte_array_ + sizeof(packet_header_.GetPacketSize()), &packet_header_.GetPacketId(), sizeof(packet_header_.GetPacketId()));
}
