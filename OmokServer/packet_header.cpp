#include "packet_header.h"

std::shared_ptr<char[]> PacketHeader::HeaderToByteArray()
{
	std::shared_ptr<char[]> buffer(new char[header_size_]);
	buffer[0] = packet_size_ & 0xFF;
	buffer[1] = (packet_size_ >> 8) & 0xFF;
	buffer[2] = packet_id_ & 0xFF;
	buffer[3] = (packet_id_ >> 8) & 0xFF;

	return buffer;
}