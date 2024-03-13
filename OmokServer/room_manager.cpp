#include "room_manager.h"

std::vector<Room> RoomManager::room_vec_;

void RoomManager::Init()
{
	for (int i = 1; i <= MAX_ROOM_NUM; ++i)
	{
		room_vec_.push_back(Room(i));
	}
}

bool RoomManager::AddSession(uint32_t session_id, uint16_t room_id)
{
	if (room_id > MAX_ROOM_NUM || room_id < 1)
	{
		return false;
	}
	room_vec_[room_id-1].AddSession(session_id);
	return true;
}

bool RoomManager::RemoveSession(uint32_t session_id, uint16_t room_id)
{
	if (room_id > MAX_ROOM_NUM || room_id < 1)
	{
		return false;
	}
	room_vec_[room_id - 1].RemoveSession(session_id);
	return true;
}

std::vector<uint32_t> RoomManager::GetSessionList(uint16_t room_id)
{
	if (room_id > MAX_ROOM_NUM || room_id < 1)
	{
		return std::vector<uint32_t>();
	}
	return room_vec_[room_id-1].GetSessionList();
}

Room* RoomManager::GetRoom(uint16_t room_id)
{
	if (room_id > MAX_ROOM_NUM || room_id < 1)
	{
		return nullptr;
	}
	return &room_vec_[room_id-1];
}

std::vector<Room*> RoomManager::GetAllRooms()
{
	std::vector<Room*> rooms;
	for (auto& room : room_vec_)
	{
		rooms.push_back(&room);
	}
	return rooms;
}
