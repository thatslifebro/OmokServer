#pragma once
#include <functional>
#include <iostream>
#include <unordered_map>
#include <bit>

#include "protobuf/OmokPacket.pb.h"
#include "packet_queue.h"
#include "packet_info.h"
#include "packet_id.h"
#include "session_manager.h"
#include "session.h"

class PacketProcessor
{
public:
	void Init();
	bool ProcessPacket();
	void ReqLoginHandler(Packet packet);

private:
	std::unordered_map<uint16_t, std::function<void(Packet) >> packet_handler_map_;

	template <typename T>
	std::tuple<char*, uint16_t> MakeResData(T packet_body);

	void SendResData(char* buffer, int length, int session_id);

	
};
