#include "packet_info.h"

std::shared_ptr<char[]> PacketHeader::HeaderToByteArray()
{
	std::shared_ptr<char[]> buffer(new char[header_size_]);
	buffer[0] = packet_size_ & 0xFF;
	buffer[1] = (packet_size_ >> 8) & 0xFF;
	buffer[2] = packet_id_ & 0xFF;
	buffer[3] = (packet_id_ >> 8) & 0xFF;

	return buffer;
}

std::tuple<std::shared_ptr<char[]>, uint16_t> Packet::ToByteArray()
{
	PacketHeader header(packet_id_, packet_size_);
	auto header_data = header.HeaderToByteArray();

	std::shared_ptr<char[]> buffer (new char[packet_size_]);
	memcpy(buffer.get(), header_data.get(), PacketHeader::header_size_);
	memcpy(buffer.get() + PacketHeader::header_size_, packet_body_, packet_size_ - PacketHeader::header_size_);

	return std::make_tuple(buffer, packet_size_);
}

void Packet::FromByteArray(std::shared_ptr<char[]> buffer)
{
	char size[2] = { buffer[0],buffer[1] };
	packet_size_ = *reinterpret_cast<uint16_t*>(size);
	char id[2] = { buffer[2],buffer[3] };
	packet_id_ = *reinterpret_cast<uint16_t*>(id);
	memcpy(packet_body_, buffer.get() + PacketHeader::header_size_, packet_size_ - PacketHeader::header_size_);
}