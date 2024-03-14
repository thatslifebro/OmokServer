#include "room.h"

void Room::AddSession(uint16_t session_id)
{
	if (session_ids_.empty())
	{
		admin_id_ = session_id;
	}
	session_ids_.insert(session_id);
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

void Room::ChangeAdmin()
{
	if (!session_ids_.empty())
	{
		admin_id_ = *session_ids_.begin();
	}
	else
	{
		admin_id_ = 0;
	}
}

void Room::TryMatchingWith(uint32_t opponent_id)
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

uint32_t Room::PlayerLeave(uint32_t session_id)
{
	if (IsGameStarted())
	{
		if (opponent_id_ == session_id || admin_id_ == session_id)
		{
			timer_->CancelTimer();
			auto opponent_id = game_->GetOpponentId(session_id);
			EndGame();
			return opponent_id;
		}
	}
	return 0;
}

bool Room::IsGameStarted() const
{
	if (game_ == nullptr)
	{
		return false;
	}
	return game_->IsGameStarted();
}


void Room::EndGame() {
	delete game_;
	game_ = nullptr;
	try_matching_ = false;
	matched_ = false;
	opponent_id_ = 0;
}