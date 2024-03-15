#pragma once
#include <vector>
#include <unordered_set>

#include "game.h"
#include "timer.h"
#include "packet_sender.h"

class Room
{
public:
	Room(uint16_t room_id) : room_id_(room_id) { timer_ = new Timer(); }

	void Init();

	void AddUser(uint16_t session_id);
	void RemoveUser(uint16_t session_id);
	std::unordered_set<uint32_t> GetSessionIds() const;

	void ChangeAdmin();
	bool IsAdmin(uint16_t session_id) const	{ return admin_id_ == session_id; }
	uint32_t GetAdminId() const { return admin_id_; }

	bool IsOpponent(uint16_t session_id) const { return opponent_id_ == session_id; }
	void TryMatchingWith(uint32_t opponent_id);

	bool IsTryMatching() const { return try_matching_; }
	bool IsTryMatchingWith(uint32_t opponent_id) const { return try_matching_ && opponent_id_ == opponent_id; }

	void Matched(uint32_t white_session_id, uint32_t black_session_id);
	bool IsMatched() const { return matched_; }
	void CancelMatch();

	Game* GetGame() { return game_; }

	bool IsGameStarted() const { return game_->IsGameStarted(); }
	uint32_t PlayerLeave(uint32_t session_id);
	void EndGame();

	void TimerCheck(uint32_t time_count) { timer_->Check(time_count); }

	void SetTimer(uint32_t time_count, uint32_t duration, std::function<void()> callback) { timer_->SetTimer(time_count, duration, callback); }

	void SetRepeatedTimer(uint32_t time_count, uint32_t duration, std::function<void()> callback) { timer_->SetRepeatedTimer(time_count, duration, callback); }

	void SetSameWithPreviousTimer(uint32_t time_count) { timer_->SetSameWithPreviousTimer(time_count); }

	void CancelTimer() { timer_->CancelTimer(); }

	//네트워크 패킷 보내는 함수
	std::function<void(uint32_t session_id)> NtfRoomUserList;
	std::function<void(uint32_t session_id)> NtfRoomUserEnter;
	std::function<void(uint32_t session_id)> NtfRoomUserLeave;
	std::function<void(uint32_t session_id)> NtfNewRoomAdmin;
	std::function<void(uint32_t session_id, std::string chat)> NtfRoomChat;
	std::function<void(uint32_t black_session_id, std::string black_user_id, uint32_t white_session_id, std::string white_user_id)> NtfStartOmokView;
	std::function<void(uint32_t session_id, uint32_t x, uint32_t y)> NtfPutMok;
	std::function<void(uint32_t winner_session_id, uint32_t loser_session_id, uint32_t result)> NtfGameOverView;

	// session_id로 부터 user_id 얻기 위한 함수
	std::function<std::string(uint32_t)> GetUserId;

	// PacketSender에게 SendPacket 함수를 넘겨주기 위한 함수
	std::function<void(uint32_t, std::shared_ptr<char[]>, uint16_t)> SendPacket;

private:
	PacketSender packet_sender_;

	Timer* timer_;

	const uint16_t room_id_;
	std::unordered_set<uint32_t> session_ids_;

	uint32_t admin_id_ = 0;
	uint32_t opponent_id_ = 0;

	bool try_matching_ = false;
	bool matched_ = false;
	
	Game* game_ = nullptr;

	void NetworkFunctionInit();
};