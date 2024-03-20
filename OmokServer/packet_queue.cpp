#include "packet_queue.h"


void PacketQueue::Save(std::shared_ptr<char[]> buffer, uint32_t length, uint32_t session_id)
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

const Packet& PacketQueue::PopAndGetPacket()
{
	if (packet_queue_.empty())
	{
		return Packet();
	}

	auto packet = packet_queue_.front();
	packet_queue_.pop();

	return packet;
}

const Packet& DBPacketQueue::PopAndGetPacket()
{
	if (db_packet_queue_.empty())
	{
		return Packet();
	}

	auto packet = db_packet_queue_.front();
	db_packet_queue_.pop();

	return packet;
}