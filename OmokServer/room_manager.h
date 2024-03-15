#pragma once
#include "room.h"
#include "constants.h"

class RoomManager
{
public:
	void Init();

	bool AddSession(uint32_t session_id, uint16_t room_id);

	bool RemoveSession(uint32_t session_id, uint16_t room_id);

	Room* GetRoom(uint16_t room_id);

	std::vector<Room*> GetAllRooms();

	std::function<std::string(uint32_t)> GetUserId;

	std::function<void(uint32_t, std::shared_ptr<char[]>, uint16_t)> SendPacket;

private:
	//TODO static 사용하지 않거나 혹은 사용해야할 이유를 알려주세요 (답변작성)
	// 
	// ROOM 객체를 정해진 수만큼 생성하고 관리하기 위해 static을 사용했습니다.
	// 데이터 베이스에 방의 정보를 저장 할 수 있다면 static을 사용하지 않아도 될 것 같습니다.
	static std::vector<Room*> room_vec_; // room_id - 1 = index
};