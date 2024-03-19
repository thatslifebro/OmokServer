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

	void InitPacketQueueFunctions(std::function<const Packet& ()> PopAndGetPacket, std::function<void(const Packet&)> PushDBPacket);

	void InitRoomManagerFunctions(std::function<void(uint32_t session_id, uint16_t room_id)> AddUser,
		std::function<void(uint32_t session_id, uint16_t room_id)> RemoveUser,
		std::function<Room* (uint16_t room_id)> GetRoom,
		std::function<std::vector<Room*>()> GetAllRooms);

	void InitSessionManagerFunctions(std::function<Session* (uint32_t session_id)> GetSession) { GetSession_ = GetSession; }

	bool ProcessPacket();

	void TimerCheck();

private:
	PacketSender packet_sender_;

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

	std::function<const Packet& ()> PopAndGetPacket_;
	std::function<void(const Packet&)> PushDBPacket_;

	std::function<void(uint32_t session_id, uint16_t room_id)> AddUser_;
	std::function<void(uint32_t session_id, uint16_t room_id)> RemoveUser_;
	std::function<Room*(uint16_t room_id)> GetRoom_;
	std::function<std::vector<Room*>()> GetAllRooms_;

	std::function<Session* (uint32_t session_id)>  GetSession_;
};
