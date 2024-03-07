#pragma once
#include <functional>
#include <iostream>
#include <unordered_map>
#include <bit>

#include "packet_queue.h"
#include "packet_info.h"
#include "session_manager.h"
#include "user_info.h"
#include "room_manager.h"
#include "packet_sender.h"

class PacketProcessor
{
public:
	void Init();
	bool ProcessPacket();
	void ReqLoginHandler(Packet packet);
	void ReqRoomEnterHandler(Packet packet);

private:
	std::unordered_map<uint16_t, std::function<void(Packet) >> packet_handler_map_;

	template <typename T>
	std::tuple<char*, uint16_t> MakeResData(PacketId packet_id, T packet_body);

	Session* GetSession(uint32_t session_id);
};
