#include "match_manager.h"

std::unordered_map<uint16_t, Game*> MatchManager::game_map_;
std::vector<uint16_t> MatchManager::match_vec_;

void MatchManager::Init()
{
	for (int i = 0; i < MAX_ROOM_NUM; i++)
	{
		match_vec_.push_back(0);
	}
}

std::tuple<uint32_t, uint16_t> MatchManager::Match(uint32_t session_id, uint16_t room_id)
{
	//���� room�� ��Ī����ڰ� �ִٸ�
	if (match_vec_[room_id - 1] != 0)
	{
		auto opponent_id = match_vec_[room_id - 1];
		match_vec_[room_id - 1] = 0;

		//game room ����
		Game* game = new Game(session_id, opponent_id);

		//game room map�� ����
		auto game_id = FindEmptyGameId();
		game_map_[game_id] = game;

		//���ӷ� �ʱ�ȭ
		game->Init();

		return std::tuple(opponent_id,game_id);
	}
	//���� room�� ��Ī����ڰ� ���ٸ�
	else
	{
		match_vec_[room_id - 1] = session_id;

		return std::tuple(0,0);
	}
}

Game* MatchManager::GetGame(uint16_t game_id)
{
	return game_map_[game_id];
}

uint16_t MatchManager::FindEmptyGameId()
{
	uint32_t game_id = 1;
	while (game_map_.find(game_id) != game_map_.end())
	{
		game_id++;
	}
	return game_id;
}

void MatchManager::GameEnd(uint16_t game_id)
{
	delete game_map_[game_id];
	game_map_.erase(game_id);
}