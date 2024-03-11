#pragma once
#include <unordered_map>

#include "game_room.h"
#include "constants.h"

class GameRoomManager
{
public:
	void Init();
	
	std::tuple<uint32_t, uint16_t> Match(uint32_t session_id, uint16_t room_id);// todo : gameroom 생성한거 delete 해야함

	GameRoom* GetGameRoom(uint16_t game_room_id);

	int FindEmptyGameRoomId();

	void SessionDisconnected(uint16_t game_room, uint32_t session_id);

private:
	static std::unordered_map<uint16_t, GameRoom*> game_room_map_;
	static std::vector<uint16_t> match_queue_vec_;
};

