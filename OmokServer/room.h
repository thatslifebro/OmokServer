#pragma once
#include <vector>
#include <unordered_set>

#include "game.h"
#include "timer.h"
#include "packet_sender.h"

class Room
{
public:
	Room(uint32_t room_id) : room_id_(room_id) {}

	void Init(std::function<void(uint32_t, char*, uint16_t)> SendPacket,
		std::function<std::string(uint32_t)> GetUserId);

	void AddUser(uint32_t session_id);
	void RemoveUser(uint32_t session_id) { session_ids_.erase(session_id); }

	bool IsAdmin(uint32_t session_id) const	{ return admin_id_ == session_id; }
	uint32_t GetAdminId() const { return admin_id_; }
	void ChangeAdmin();
	bool IsEmpty() const { return session_ids_.empty(); }

	bool IsOpponent(uint32_t session_id) const { return opponent_id_ == session_id; }
	void TryMatchingWith(uint32_t opponent_id);

	bool IsTryMatching() const { return try_matching_; }
	bool IsTryMatchingWith(uint32_t opponent_id) const { return try_matching_ && opponent_id_ == opponent_id; }

	void MatchComplete(uint32_t white_session_id, uint32_t black_session_id);
	bool IsMatched() const { return matched_; }
	void CancelMatch();

	Game* GetGame() { return game_; }
	std::tuple<uint32_t, std::string, uint32_t, std::string> GetPlayerInfo() const;

	bool IsGameStarted() const { return game_->IsGameStarted(); }
	bool IsPlayer(uint32_t session_id);
	uint32_t GetOpponentPlayer(uint32_t session_id);
	void EndMatch();

	void TimerCheck(uint32_t time_count) { timer_->Check(time_count); }

	void SetTimer(uint32_t time_count, uint16_t duration, std::function<void()> callback) { timer_->SetTimer(time_count, duration, callback); }

	void SetRepeatedTimer(uint32_t time_count, uint16_t duration, std::function<void()> callback) { timer_->SetRepeatedTimer(time_count, duration, callback); }

	void SetSameWithPreviousTimer(uint32_t time_count) { timer_->SetSameWithPreviousTimer(time_count); }

	void CancelTimer() { timer_->CancelTimer(); }

	// packet sender
	void ResRoomUserList(uint32_t session_id);
	void NtfRoomUserEnter(uint32_t session_id);
	void NtfRoomUserLeave(uint32_t session_id);
	void NtfNewRoomAdmin();
	void NtfRoomChat(uint32_t session_id, std::string chat);
	void NtfStartOmokView(uint32_t black_session_id, std::string black_user_id, uint32_t white_session_id, std::string white_user_id);
	void NtfPutMok(uint32_t session_id, uint32_t x, uint32_t y);
	void NtfGameOverView(uint32_t winner_session_id, uint32_t loser_session_id);
	void NtfPutMokTimeout();

private:
	PacketSender packet_sender_;

	Timer* timer_ = nullptr;

	const uint32_t room_id_;
	std::unordered_set<uint32_t> session_ids_;

	uint32_t admin_id_ = 0;
	uint32_t opponent_id_ = 0;

	bool try_matching_ = false;
	bool matched_ = false;
	
	Game* game_ = nullptr;

	std::function<std::string(uint32_t)> GetUserId_;
};