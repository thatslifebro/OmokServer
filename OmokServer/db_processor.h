#pragma once
#include <unordered_map>
#include <functional>

#include "session.h"
#include "error_code.h"
#include "packet_sender.h"

class DBProcessor
{
public:
	void Init();

	void InitDBPacketQueueFunctions(std::function<Packet()> PopAndGetPacket) { PopAndGetPacket_ = PopAndGetPacket; }

	void InitSessionManagerFunctions(std::function<Session* (uint32_t session_id)> GetSession) { GetSession_ = GetSession; }

	bool ProcessDBPacket();

private:
	std::unordered_map<uint16_t, std::function<ErrorCode(Packet) >> packet_handler_map_;
	PacketSender packet_sender_;

	ErrorCode ReqLoginHandler(Packet packet);
	std::string ReqLoginPacketData(Packet packet);
	void ReqLoginProcess(Session* session, std::string user_id);

	bool IsValidSession(Session* session);

	std::function<Packet()> PopAndGetPacket_;

	std::function<Session* (uint32_t session_id)>  GetSession_;
};