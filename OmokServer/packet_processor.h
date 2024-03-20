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

	void InitPacketQueueFunctions(std::function<Packet()> PopAndGetPacket, std::function<void(Packet)> PushDBPacket);

	void InitRoomManagerFunctions(std::function<void(uint32_t session_id, uint32_t room_id)> AddUser,
		std::function<void(uint32_t session_id, uint32_t room_id)> RemoveUser,
		std::function<Room* (uint32_t room_id)> GetRoom,
		std::function<std::vector<Room*>()> GetAllRooms);

	void InitSessionManagerFunctions(std::function<Session* (uint32_t session_id)> GetSession,
		std::function<void(uint32_t)> RemoveSession) { GetSession_ = GetSession; RemoveSession_ = RemoveSession; }

	bool ProcessPacket();

	void TimerCheck();

private:
	PacketSender packet_sender_;

	std::unordered_map<uint16_t, std::function<ErrorCode(Packet) >> packet_handler_map_;

	int time_count_ = 0;

	ErrorCode ReqLoginHandler(Packet packet);

	ErrorCode ReqRoomEnterHandler(Packet packet);

	uint32_t ReqRoomEnterPacketData(Packet packet);
	void ReqRoomEnterProcess(uint32_t session_id, Session* session, std::string user_id, uint32_t room_id, Room* room);
	ErrorCode ReqRoomEnterErrorCheck(Session* session, uint32_t session_id, std::string user_id, Room* room);

	ErrorCode ReqRoomLeaveHandler(Packet packet);

	void ReqRoomLeaveProcess(Session* session, uint32_t session_id, Room* room, uint32_t room_id);
	ErrorCode ReqRoomLeaveErrorCheck(Session* session, uint32_t session_id);

	ErrorCode ReqRoomChatHandler(Packet packet);

	std::string ReqRoomChatPacketData(Packet packet);
	void ReqRoomChatProcess(uint32_t session_id, std::string chat, Room* room);
	ErrorCode ReqRoomChatErrorCheck(Session* session, uint32_t session_id);

	ErrorCode ReqMatchHandler(Packet packet);

	uint32_t ReqMatchPacketData(Packet packet);
	void ReqMatchProcess(uint32_t session_id, Session* session, uint32_t opponent_id, Session* opponent_session, Room* room);
	ErrorCode ReqMatchErrorCheck(uint32_t session_id, Session* session, uint32_t opponent_id, Session* opponent_session, Room* room);

	ErrorCode ReqMatchResHandler(Packet packet);

	bool ReqMatchResPacketData(Packet packet);
	void ReqMatchResProcess(uint32_t session_id, Session* session, uint32_t admin_id, bool accept, Room* room);
	ErrorCode ReqMatchResErrorCheck(uint32_t session_id, uint32_t admin_id, Room* room);

	ErrorCode ReqReadyOmokHandler(Packet packet);

	void ReqReadyOmokProcess(uint32_t session_id, Session* session, Room* room, Game* game);
	ErrorCode ReqReadyOmokErrorCheck(uint32_t session_id, Session* session, Room* room, Game* game);

	ErrorCode ReqOmokPutHandler(Packet packet);

	std::tuple<uint32_t, uint32_t> ReqOmokPutPacketData(Packet packet);
	void ReqOmokPutProcess(uint32_t session_id, Room* room, Game* game, uint32_t x, uint32_t y);
	ErrorCode ReqOmokPutErrorCheck(uint32_t session_id, Room* room, Game* game, uint32_t x, uint32_t y);

	ErrorCode ReqRemoveSession(Packet packet);

	bool IsValidSession(Session* session);

	bool IsValidRoom(Room* room);

	std::function<Packet()> PopAndGetPacket_;
	std::function<void(Packet)> PushDBPacket_;

	std::function<void(uint32_t session_id, uint32_t room_id)> AddUser_;
	std::function<void(uint32_t session_id, uint32_t room_id)> RemoveUser_;
	std::function<Room*(uint32_t room_id)> GetRoom_;
	std::function<std::vector<Room*>()> GetAllRooms_;

	std::function<Session* (uint32_t session_id)>  GetSession_;
	std::function<void(uint32_t)> RemoveSession_;
};
