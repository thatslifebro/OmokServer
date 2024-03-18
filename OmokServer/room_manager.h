#pragma once
#include "room.h"
#include "constants.h"

class RoomManager
{
public:
	void Init(uint16_t max_room_num, std::function<void(uint32_t session_id, std::shared_ptr<char[]> buffer, int length)> SendPacket, std::function<std::string(uint32_t session_id)> GetUserId);

	void AddUser(uint32_t session_id, uint16_t room_id) { room_vec_[room_id - 1]->AddUser(session_id);  }

	void RemoveUser(uint32_t session_id, uint16_t room_id) { room_vec_[room_id - 1]->RemoveUser(session_id); }

	Room* GetRoom(uint16_t room_id);

	std::vector<Room*> GetAllRooms();

private:
	static uint16_t max_room_num_;
	//TODO static 사용하지 않거나 혹은 사용해야할 이유를 알려주세요 (답변작성)
	// 
	// room_vec_를 이용해 Room에 접근하는 곳이 DBProcessor, PacketProcessor, Session 인데
	// 모두 같은 Room 정보를 얻기 위해 static으로 선언했습니다.
	static std::vector<Room*> room_vec_; // room_id - 1 = index
};