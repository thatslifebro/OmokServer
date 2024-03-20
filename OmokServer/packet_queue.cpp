#include "packet_queue.h"

void PacketQueue::SaveByteArray(std::shared_ptr<char[]> buffer, uint32_t length, uint32_t session_id)
{
	if (length < PacketHeader::HEADER_SIZE)
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
	if (packet_queue_.empty())
	{
		return Packet();
	}

	auto packet = packet_queue_.front();
	packet_queue_.pop();

	return packet;
}

Packet DBPacketQueue::PopAndGetPacket()
{
	if (db_packet_queue_.empty())
	{
		return Packet();
	}

	auto packet = db_packet_queue_.front();
	db_packet_queue_.pop();

	return packet;
}