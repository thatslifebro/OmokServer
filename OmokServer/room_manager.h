#pragma once
#include <set>
#include <vector>

#include "Room.h"

static constexpr uint16_t MAX_ROOM_NUM = 10;

class RoomManager
{
public:
	static void Init();
	static bool AddSession(uint32_t session_id, uint16_t room_id);
	static void RemoveSession(uint32_t session_id, uint16_t room_id);
	static std::vector<uint32_t> GetSessionList(uint16_t room_id);
private:
	static std::vector<Room> room_vec_;
};