#include "room_manager.h"

void RoomManager::Init(uint16_t max_room_num,
	std::function<void(uint32_t session_id, char* buffer, int length)> SendPacket,
	std::function<std::string(uint32_t session_id)> GetUserId)
{
	max_room_num_ = max_room_num;
	for (int i = 1; i <= max_room_num_; ++i)
	{
		auto room = new Room(i);
		room->Init(SendPacket, GetUserId);

		room_vec_.push_back(room);
	}
}

Room* RoomManager::GetRoom(uint16_t room_id)
{
	if (room_id > max_room_num_ || room_id < 1)
	{
		return nullptr;
	}
	return room_vec_[room_id-1];
}

std::vector<Room*> RoomManager::GetAllRooms()
{
	return room_vec_;
}
