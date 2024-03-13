#pragma once
#include <vector>
#include <set>

#include "game.h"

class Room
{
public:
	Room(uint16_t room_id) : room_id_(room_id) {}

	void AddSession(uint16_t session_id);

	void RemoveSession (uint16_t session_id) { session_ids_.erase(session_id); }

	bool ChangeAdmin(uint16_t session_id);

	bool IsAdmin(uint16_t session_id) const	{ return admin_id_ == session_id; }

	bool IsOpponent(uint16_t session_id) const { return opponent_id_ == session_id; }

	uint32_t GetAdminId() const	{ return admin_id_; }

	std::vector<uint32_t> GetSessionList() const;

	bool IsGameStarted() const;

	void TryMatching(uint32_t opponent_id);

	bool IsTryMatching() const { return try_matching_; }

	bool IsTryMatchingWith(uint32_t opponent_id) const { return try_matching_ && opponent_id_ == opponent_id; }

	void CancelMatch();

	void StartGame() { game_ = new Game(admin_id_, opponent_id_); }

	Game* GetGame() { return game_; }

	void EndGame();

	bool IsMatched() const { return matched_; }

	void Matched() { matched_ = true; }

private:
	std::set<uint16_t> session_ids_;
	const uint16_t room_id_;
	uint32_t admin_id_ = 0;
	bool is_game_started_ = false;
	bool try_matching_ = false;
	bool matched_ = false;
	uint32_t opponent_id_ = 0;
	Game* game_ = nullptr;
};