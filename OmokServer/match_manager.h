#pragma once
#include <unordered_map>

#include "game.h"
#include "constants.h"

class MatchManager
{
public:
	void Init();
	
	std::tuple<uint32_t, uint16_t> Match(uint32_t session_id, uint16_t room_id);

	Game* GetGame(uint16_t game_id);

	uint16_t FindEmptyGameId();

	void GameEnd(uint16_t game_id);

private:
	static std::unordered_map<uint16_t, Game*> game_map_;
	static std::vector<uint16_t> match_vec_;
};

