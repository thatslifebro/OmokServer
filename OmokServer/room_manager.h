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
	//TODO static ������� �ʰų� Ȥ�� ����ؾ��� ������ �˷��ּ��� (�亯�ۼ�)
	// 
	// ROOM ��ü�� ������ ����ŭ �����ϰ� �����ϱ� ���� static�� ����߽��ϴ�.
	// ������ ���̽��� ���� ������ ���� �� �� �ִٸ� static�� ������� �ʾƵ� �� �� �����ϴ�.
	static std::vector<Room*> room_vec_; // room_id - 1 = index
};