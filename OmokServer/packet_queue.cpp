#include "packet_queue.h"

std::mutex PacketQueue::mutex_;
std::queue<Packet> PacketQueue::packet_queue_;

void PacketQueue::Save(std::shared_ptr<char[]> buffer, uint32_t length, uint32_t session_id)
{
	if (length < PacketHeader::header_size_)
	{
		return;
	}

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