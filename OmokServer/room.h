#pragma once
#include <vector>
#include <set>

class Room
{
public:
	Room(uint16_t room_id) : room_id_(room_id) {}

	void AddSession (uint16_t session_id) { session_ids_.insert(session_id); }

	void RemoveSession (uint16_t session_id) { session_ids_.erase(session_id); }

	std::vector<uint32_t> GetSessionList() const;

private:
	std::set<uint16_t> session_ids_;
	const uint16_t room_id_;
};