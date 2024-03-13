#pragma once
#include <functional>
#include <unordered_map>

#include "packet_queue.h"
#include "session_manager.h"
#include "room_manager.h"
#include "db_processor.h"

class PacketProcessor
{
public:
	PacketQueue packet_queue_;
	DBPacketQueue db_packet_queue_;
	PacketSender packet_sender_;
	RoomManager room_manager_;
	SessionManager session_manager_;
	DBProcessor db_processor_;

	void Init();

	bool ProcessPacket();

private:
	std::unordered_map<uint16_t, std::function<void(Packet) >> packet_handler_map_;

	void ReqLoginHandler(Packet packet);

	void ReqRoomEnterHandler(Packet packet);

	void ReqRoomLeaveHandler(Packet packet);

	void ReqRoomChatHandler(Packet packet);

	void ReqMatchHandler(Packet packet);

	void ReqMatchRes(Packet packet);

	void ReqReadyOmokHandler(Packet packet);

	void ReqOmokPutHandler(Packet packet);
};
