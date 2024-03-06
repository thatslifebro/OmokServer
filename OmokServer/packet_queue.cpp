#include "packet_queue.h"

std::mutex PacketQueue::mutex_;
std::queue<Packet> PacketQueue::packet_queue_;

void PacketQueue::Save(char* buffer, int session_id)
{
	Packet packet;
	packet.session_id_ = session_id;
	packet.FromByteArray(buffer);

	std::lock_guard<std::mutex> lock(mutex_);
	packet_queue_.push(packet);
}

const Packet& PacketQueue::PopAndGetPacket()
{
	std::lock_guard<std::mutex> lock(mutex_);

	if (packet_queue_.empty())
	{
		return Packet();
	}

	auto packet = packet_queue_.front();
	packet_queue_.pop();

	return packet;
}