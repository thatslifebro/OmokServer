#pragma once
#include <unordered_map>
#include <string>
#include <queue>
#include <mutex>
#include <functional>

#include "session_manager.h"
#include "packet_sender.h"
#include "packet_queue.h"

class DBProcessor
{
public:
	void Init(); // DB 연결

	bool ProcessDB(); // DB쓰레드에서 실행될 함수

private:
	std::unordered_map<uint16_t, std::function<void(Packet) >> packet_handler_map_;
	DBPacketQueue db_packet_queue_;
	SessionManager session_manager_;
	PacketSender packet_sender_;

	void ReqLoginHandler(Packet packet);
};