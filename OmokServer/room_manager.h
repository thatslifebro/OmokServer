#pragma once
#include "room.h"

class RoomManager
{
public:
	void Init(uint16_t max_room_num,
		std::function<void(uint32_t session_id, char* buffer, int length)> SendPacket,
		std::function<std::string(uint32_t session_id)> GetUserId);

	void UserEnterRoom(uint32_t session_id, uint32_t room_id) { room_vec_[room_id - 1]->AddUser(session_id);  }

	void UserLeaveRoom(uint32_t session_id, uint32_t room_id) { room_vec_[room_id - 1]->RemoveUser(session_id); }

	Room* GetRoom(uint32_t room_id);

	std::vector<Room*> GetAllRooms();

private:
	uint16_t max_room_num_;
	std::vector<Room*> room_vec_; // room_id - 1 = index
};