#include "packet_queue.h"


void PacketQueue::Save(std::shared_ptr<char[]> buffer, uint32_t length, uint32_t session_id)
{
	if (length < PacketHeader::header_size_)
	{
		return;
	}

	Packet packet;
	packet.SetSessionId(session_id);
	packet.FromByteArray(buffer);

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

// DBPacketQueue
std::mutex DBPacketQueue::db_mutex_;
std::queue<Packet> DBPacketQueue::db_packet_queue_;

void DBPacketQueue::PushPacket(const Packet& packet)
{
	std::lock_guard<std::mutex> lock(db_mutex_);
	db_packet_queue_.push(packet);
}

const Packet& DBPacketQueue::PopAndGetPacket()
{
	std::lock_guard<std::mutex> lock(db_mutex_);

	if (db_packet_queue_.empty())
	{
		return Packet();
	}

	auto packet = db_packet_queue_.front();
	db_packet_queue_.pop();

	return packet;
}