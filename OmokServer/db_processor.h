#pragma once
#include <unordered_map>
#include <string>
#include <queue>
#include <mutex>
#include <functional>

#include "session.h"
#include "packet_queue.h"

class DBProcessor
{
public:
	void Init();

	void InitDBPacketQueueFunctions(std::function<const Packet& ()> PopAndGetPacket) { PopAndGetPacket_ = PopAndGetPacket; }

	void InitSessionManagerFunctions(std::function<Session* (uint32_t session_id)> GetSession) { GetSession_ = GetSession; }

	bool ProcessDB();

private:
	std::unordered_map<uint16_t, std::function<void(Packet) >> packet_handler_map_;
	PacketSender packet_sender_;

	void ReqLoginHandler(Packet packet);

	std::function<const Packet& ()> PopAndGetPacket_;

	std::function<Session* (uint32_t session_id)>  GetSession_;
};