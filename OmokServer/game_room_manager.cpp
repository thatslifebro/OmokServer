#include "game_room_manager.h"

std::unordered_map<uint16_t, GameRoom*> GameRoomManager::game_room_map_;
std::vector<uint16_t> GameRoomManager::match_queue_vec_;

void GameRoomManager::Init()
{
	for (int i = 0; i < MAX_ROOM_NUM; i++)
	{
		match_queue_vec_.push_back(0);
	}
}

std::tuple<uint32_t, uint16_t> GameRoomManager::Match(uint32_t session_id, uint16_t room_id)
{
	//같은 room에 매칭대기자가 있다면
	if (match_queue_vec_[room_id - 1] != 0)
	{
		auto opponent_id = match_queue_vec_[room_id - 1];
		match_queue_vec_[room_id - 1] = 0;

		//game room 생성
		GameRoom* game_room = new GameRoom(session_id, opponent_id);

		//game room map에 저장
		auto game_room_id = FindEmptyGameRoomId();
		game_room_map_[game_room_id] = game_room;

		//게임룸 초기화
		game_room->Init();

		return std::tuple(opponent_id,game_room_id);
	}
	//같은 room에 매칭대기자가 없다면
	else
	{
		match_queue_vec_[room_id - 1] = session_id;

		return std::tuple(0,0);
	}
}

GameRoom* GameRoomManager::GetGameRoom(uint16_t game_room_id)
{
	return game_room_map_[game_room_id];
}

uint16_t GameRoomManager::FindEmptyGameRoomId()
{
	uint32_t game_room_id = 1;
	while (game_room_map_.find(game_room_id) != game_room_map_.end())
	{
		game_room_id++;
	}
	return game_room_id;
}

void GameRoomManager::GameEnd(uint16_t game_room_id)
{
	delete game_room_map_[game_room_id];
	game_room_map_.erase(game_room_id);
}