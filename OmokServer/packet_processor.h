#pragma once
#include <functional>
#include <unordered_map>

#include "packet_queue.h"
#include "room_manager.h"
#include "session.h"

class PacketProcessor
{
public:
	PacketSender packet_sender_;
	RoomManager room_manager_;
	SessionManager session_manager_;

	void Init();

	bool ProcessPacket();

	void TimerCheck();

private:
	std::unordered_map<uint16_t, std::function<void(Packet) >> packet_handler_map_;

	int time_count_ = 0;

	void ReqLoginHandler(Packet packet);

	void ReqRoomEnterHandler(Packet packet);

	void ReqRoomLeaveHandler(Packet packet);

	void ReqRoomChatHandler(Packet packet);

	void ReqMatchHandler(Packet packet);

	void ReqMatchRes(Packet packet);

	void ReqReadyOmokHandler(Packet packet);

	void ReqOmokPutHandler(Packet packet);
};
