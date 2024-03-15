#include "room_manager.h"

std::vector<Room*> RoomManager::room_vec_;
uint16_t RoomManager::max_room_num_;

void RoomManager::Init(uint16_t max_room_num)
{
	max_room_num_ = max_room_num;
	for (int i = 1; i <= max_room_num_; ++i)
	{
		auto room = new Room(i);
		room->GetUserId = GetUserId;
		room->SendPacket = SendPacket;
		room->Init();

		room_vec_.push_back(room);
	}
}

bool RoomManager::AddSession(uint32_t session_id, uint16_t room_id)
{
	if (room_id > max_room_num_ || room_id < 1)
	{
		return false;
	}
	room_vec_[room_id-1]->AddUser(session_id);
	return true;
}

bool RoomManager::RemoveSession(uint32_t session_id, uint16_t room_id)
{
	if (room_id > max_room_num_ || room_id < 1)
	{
		return false;
	}
	room_vec_[room_id - 1]->RemoveUser(session_id);
	return true;
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
