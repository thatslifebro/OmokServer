#pragma once
#include <unordered_map>

#include "room_manager.h"
#include "session_manager.h"
#include "game_room.h"


class GameRoomManager
{
public:
	void Init();
	
	uint32_t Match(Session* session);

	GameRoom GetGameRoom(uint16_t game_room_id);

	int FindEmptyGameRoomId();

private:
	SessionManager session_manager_;

	static std::unordered_map<uint16_t, GameRoom> game_room_map_;
	static std::vector<uint16_t> match_queue_vec_;
};

