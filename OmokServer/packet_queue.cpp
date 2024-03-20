#include "packet_queue.h"

void PacketQueue::SaveByteArray(std::shared_ptr<char[]> buffer, uint32_t length, uint32_t session_id)
{
	if (length < PacketHeader::HEADER_SIZE || length > MAX_PACKET_SIZE)
	{
		return;
	}

	Packet packet;
	packet.SetSessionId(session_id);
	packet.ParseData(buffer);

	packet_queue_.push(packet);
}

void PacketQueue::SavePacket(Packet packet)
{
	packet_queue_.push(packet);
}

Packet PacketQueue::PopAndGetPacket()
{
	Packet packet;
	if (packet_queue_.try_pop(packet))
	{
		return packet;
	}

	return Packet();
}