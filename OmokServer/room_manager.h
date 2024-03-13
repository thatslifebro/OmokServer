#pragma once
#include "room.h"
#include "constants.h"

class RoomManager
{
public:
	void Init();

	bool AddSession(uint32_t session_id, uint16_t room_id);

	bool RemoveSession(uint32_t session_id, uint16_t room_id);

	std::vector<uint32_t> GetSessionList(uint16_t room_id);

	Room* GetRoom(uint16_t room_id);

	std::vector<Room*> GetAllRooms();

private:
	static std::vector<Room> room_vec_; // room_id - 1 = index
};