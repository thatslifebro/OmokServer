#pragma once
#include <vector>
#include <set>

#include "game.h"
#include "timer.h"

class Room
{
public:
	Room(uint16_t room_id) : room_id_(room_id) { timer_ = new Timer(); }

	void AddSession(uint16_t session_id);

	void RemoveSession (uint16_t session_id) { session_ids_.erase(session_id); }

	std::vector<uint32_t> GetSessionList() const;

	void ChangeAdmin();

	bool IsAdmin(uint16_t session_id) const	{ return admin_id_ == session_id; }

	uint32_t GetAdminId() const { return admin_id_; }

	bool IsOpponent(uint16_t session_id) const { return opponent_id_ == session_id; }

	void TryMatchingWith(uint32_t opponent_id);

	bool IsTryMatching() const { return try_matching_; }

	bool IsTryMatchingWith(uint32_t opponent_id) const { return try_matching_ && opponent_id_ == opponent_id; }

	void Matched() { matched_ = true; }

	bool IsMatched() const { return matched_; }

	void CancelMatch();

	void CreateGame();

	Game* GetGame() { return game_; }

	uint32_t PlayerLeave(uint32_t session_id);

	bool IsGameStarted() const;

	void EndGame();

	void TimerCheck(uint32_t time_count);

	void SetTimer(uint32_t time_count, uint32_t duration, std::function<void()> callback);

	void SetRepeatedTimer(uint32_t time_count, uint32_t duration, std::function<void()> callback);

	void SetSameWithPreviousTimer(uint32_t time_count);

	void CancelTimer();

	void ContinueTimer();

private:
	Timer* timer_;

	const uint16_t room_id_;
	std::set<uint16_t> session_ids_;

	uint32_t admin_id_ = 0;
	uint32_t opponent_id_ = 0;

	bool try_matching_ = false;
	bool matched_ = false;
	
	Game* game_ = nullptr;
};