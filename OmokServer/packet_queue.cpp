#include "packet_queue.h"

std::mutex PacketQueue::mutex_;
std::queue<Packet> PacketQueue::packet_queue_;