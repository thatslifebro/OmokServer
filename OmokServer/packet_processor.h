#pragma once
#include <functional>
#include <unordered_map>

#include "packet_queue.h"
#include "room_manager.h"
#include "session.h"
#include "error_code.h"

class PacketProcessor
{
public:
	void Init();

	bool ProcessPacket();

	void TimerCheck();

	std::function<Packet()> PopAndGetPacket;

private:
	PacketSender packet_sender_;
	RoomManager room_manager_;
	SessionManager session_manager_;

	std::unordered_map<uint16_t, std::function<ErrorCode(Packet) >> packet_handler_map_;

	int time_count_ = 0;

	ErrorCode ReqLoginHandler(Packet packet);

	ErrorCode ReqRoomEnterHandler(Packet packet);

	ErrorCode ReqRoomLeaveHandler(Packet packet);

	ErrorCode ReqRoomChatHandler(Packet packet);

	ErrorCode ReqMatchHandler(Packet packet);

	ErrorCode ReqMatchResHandler(Packet packet);

	ErrorCode ReqReadyOmokHandler(Packet packet);

	ErrorCode ReqOmokPutHandler(Packet packet);

	bool IsValidSession(Session* session);

	bool IsValidRoom(Room* room);	
};
