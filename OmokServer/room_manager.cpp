#include "room_manager.h"

std::vector<Room*> RoomManager::room_vec_;

void RoomManager::Init()
{
	for (int i = 1; i <= MAX_ROOM_NUM; ++i)
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
	if (room_id > MAX_ROOM_NUM || room_id < 1)
	{
		return false;
	}
	room_vec_[room_id-1]->AddSession(session_id);
	return true;
}

bool RoomManager::RemoveSession(uint32_t session_id, uint16_t room_id)
{
	if (room_id > MAX_ROOM_NUM || room_id < 1)
	{
		return false;
	}
	room_vec_[room_id - 1]->RemoveSession(session_id);
	return true;
}

Room* RoomManager::GetRoom(uint16_t room_id)
{
	if (room_id > MAX_ROOM_NUM || room_id < 1)
	{
		return nullptr;
	}
	return room_vec_[room_id-1];
}

std::vector<Room*> RoomManager::GetAllRooms()
{
	return room_vec_;
}
