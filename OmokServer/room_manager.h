#pragma once
#include "room.h"
#include "constants.h"

class RoomManager
{
public:
	void Init();

	bool AddSession(uint32_t session_id, uint16_t room_id);

	bool RemoveSession(uint32_t session_id, uint16_t room_id);

	//TODO 
	std::vector<uint32_t> GetSessionList(uint16_t room_id);

	Room* GetRoom(uint16_t room_id);

	std::vector<Room*> GetAllRooms();

private:
	//TODO static 사용하지 않거나 혹은 사용해야할 이유를 알려주세요
	static std::vector<Room> room_vec_; // room_id - 1 = index
};