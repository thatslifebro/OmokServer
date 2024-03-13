#include "room.h"

void Room::AddSession(uint16_t session_id)
{
	if (session_ids_.empty())
	{
		admin_id_ = session_id;
	}
	session_ids_.insert(session_id);
}

bool Room::ChangeAdmin(uint16_t session_id)
{
	if (admin_id_ == session_id)
	{
		if (!session_ids_.empty())
		{
			admin_id_ = *session_ids_.begin();
			return true;
		}
		else
		{
			admin_id_ = 0;
		}
	}
	return false;
}

std::vector<uint32_t> Room::GetSessionList() const
{
	std::vector<uint32_t> session_list;

	for (auto session_id : session_ids_)
	{
		session_list.push_back(session_id);
	}

	return session_list;
}

bool Room::IsGameStarted() const
{
	if (game_ == nullptr)
	{
		return false;
	}
	return game_->IsGameStart();
}

void Room::TryMatching(uint32_t opponent_id)
{
	opponent_id_ = opponent_id;
	try_matching_ = true;
}

void Room::CancelMatch()
{
	try_matching_ = false;
	matched_ = false;
	opponent_id_ = 0;
}

void Room::EndGame() {
	delete game_;
	game_ = nullptr;
	is_game_started_ = false;
	try_matching_ = false;
	matched_ = false;
	opponent_id_ = 0;
}