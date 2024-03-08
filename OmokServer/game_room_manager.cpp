#include "game_room_manager.h"

std::unordered_map<uint16_t, GameRoom*> GameRoomManager::game_room_map_;
std::vector<uint16_t> GameRoomManager::match_queue_vec_;

void GameRoomManager::Init()
{
	game_room_map_.clear();
	match_queue_vec_.clear();
	for (int i = 0; i < MAX_ROOM_NUM; i++)
	{
		match_queue_vec_.push_back(0);
	}
}

uint32_t GameRoomManager::Match(Session* session)
{
	auto room_id = session->session_room_id_;
	auto session_id = session->session_id_;

	//���� room�� ��Ī����ڰ� �ִٸ�
	if (match_queue_vec_[room_id - 1] != 0)
	{
		auto opponent_id = match_queue_vec_[room_id - 1];
		match_queue_vec_[room_id - 1] = 0;

		//game room ����
		GameRoom* game_room = new GameRoom(session_id, opponent_id);

		//game room map�� ����
		auto game_room_id = FindEmptyGameRoomId();
		game_room_map_[game_room_id] = game_room;

		//session�� game room id ����
		session->game_room_id_ = game_room_id;
		auto oponnent_session = session_manager_.GetSession(opponent_id);
		oponnent_session->game_room_id_ = game_room_id;

		//���ӷ� �ʱ�ȭ
		game_room->Init();

		return opponent_id;
	}
	//���� room�� ��Ī����ڰ� ���ٸ�
	else
	{
		match_queue_vec_[room_id - 1] = session_id;

		return 0;
	}
}

GameRoom* GameRoomManager::GetGameRoom(uint16_t game_room_id)
{
	return game_room_map_[game_room_id];
}

int GameRoomManager::FindEmptyGameRoomId()
{
	uint32_t game_room_id = 1;
	while (game_room_map_.find(game_room_id) != game_room_map_.end())
	{
		game_room_id++;
	}
	return game_room_id;
}