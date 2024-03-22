#pragma once
#include <functional>
#include <unordered_map>

#include "session.h"
#include "room.h"
#include "error_code.h"

#include "packet_handler/room_enter_handler.h"
#include "packet_handler/room_leave_handler.h"
#include "packet_handler/room_chat_handler.h"
#include "packet_handler/match_req_handler.h"
#include "packet_handler/match_res_handler.h"
#include "packet_handler/ready_handler.h"
#include "packet_handler/put_mok_handler.h"

class PacketProcessor
{
public:
	void Init();
	void InitPacketHandler();
	void InitSaveHandleRequestFunc();

	void InitPacketQueueFunctions(std::function<Packet()> PopAndGetPacket, std::function<void(Packet)> PushDBPacket);

	void InitRoomManagerFunctions(std::function<void(uint32_t session_id, uint32_t room_id)> UserEnterRoom,
		std::function<void(uint32_t session_id, uint32_t room_id)> UserLeaveRoom,
		std::function<Room* (uint32_t room_id)> GetRoom,
		std::function<std::vector<Room*>()> GetAllRooms);

	void InitSessionManagerFunctions(std::function<uint32_t(Session*)> AddSession,
		std::function<Session* (uint32_t session_id)> GetSession,
		std::function<void(uint32_t)> RemoveSession);

	bool ProcessPacket();

	void TimerCheck();

private:
	PacketSender packet_sender_;

	RoomEnterHandler room_enter_handler_;
	RoomLeaveHandler room_leave_handler_;
	RoomChatHandler room_chat_handler_;
	MatchReqHandler match_req_handler_;
	MatchResHandler match_res_handler_;
	ReadyHandler ready_handler_;
	PutMokHandler put_mok_handler_;

	std::unordered_map<uint16_t, std::function<ErrorCode(Packet) >> packet_handler_map_;

	uint32_t time_count_ = 0;

	ErrorCode ReqLoginHandler(Packet packet);

	ErrorCode ReqAddSession(Packet packet);

	ErrorCode ReqRemoveSession(Packet packet);
	void ReqRemoveSessionRoomLeaveProcess(Session* session, uint32_t session_id, Room* room, uint32_t room_id);
	void NotifyOthersEndGame(Room* room, uint32_t room_id, uint32_t session_id);
	void ChangeAdminProcess(Room* room);

	std::function<Packet()> PopAndGetPacket_;
	std::function<void(Packet)> PushDBPacket_;

	std::function<void(uint32_t session_id, uint32_t room_id)> UserEnterRoom_;
	std::function<void(uint32_t session_id, uint32_t room_id)> UserLeaveRoom_;
	std::function<Room*(uint32_t room_id)> GetRoom_;
	std::function<std::vector<Room*>()> GetAllRooms_;

	std::function<uint32_t(Session*)> AddSession_;
	std::function<Session* (uint32_t session_id)>  GetSession_;
	std::function<void(uint32_t)> RemoveSession_;
};
