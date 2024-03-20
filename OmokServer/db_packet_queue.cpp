#include "db_packet_queue.h"

Packet DBPacketQueue::PopAndGetPacket()
{
	Packet packet;
	if (db_packet_queue_.try_pop(packet))
	{
		return packet;
	}

	return Packet();
}