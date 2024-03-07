#pragma once
#include "Room.h"

static constexpr uint16_t MAX_ROOM_NUM = 10;

class RoomManager
{
public:
	void Init();
	bool AddSession(uint32_t session_id, uint16_t room_id);
	void RemoveSession(uint32_t session_id, uint16_t room_id);
	std::vector<uint32_t> GetSessionList(uint16_t room_id);
private:
	static std::vector<Room> room_vec_;
};