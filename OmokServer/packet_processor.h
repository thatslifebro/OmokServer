#pragma once
#include <functional>
#include <iostream>
#include <unordered_map>
#include <bit>

#include "packet_queue.h"
#include "packet_info.h"
#include "session_manager.h"
#include "user_info.h"
#include "room_manager.h"
#include "packet_sender.h"
#include "game_room_manager.h"
#include "db_processor.h"

class PacketProcessor
{
public:
	PacketQueue packet_queue_;
	PacketSender packet_sender_;
	RoomManager room_manager_;
	SessionManager session_manager_;
	GameRoomManager game_room_manager_;
	DBProcessor db_processor_;

	void Init();

	bool ProcessPacket();

	void ReqLoginHandler(Packet packet);

	void ReqRoomEnterHandler(Packet packet);

	void ReqRoomLeaveHandler(Packet packet);

	void ReqRoomChatHandler(Packet packet);

	void ReqMatchHandler(Packet packet);

	void ReqReadyOmokHandler(Packet packet);

	void ReqOmokPutHandler(Packet packet);

private:
	std::unordered_map<uint16_t, std::function<void(Packet) >> packet_handler_map_;
};
