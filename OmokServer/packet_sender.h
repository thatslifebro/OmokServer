#pragma once
#include "protobuf/OmokPacket.pb.h"
#include "session.h"
#include "packet_id.h"

class PacketSender
{
public:
	static void ResLogin(Session* session, uint32_t result);
	static void ResRoomEnter(Session* session, uint32_t result);
	static void BroadcastRoomUserEnter(std::vector<uint32_t> room_session_ids, Session* session);
	static void NtfRoomUserList(Session* session, std::vector<uint32_t> room_session_ids);

private:
	template <typename T>
	static std::tuple<char*, uint16_t> MakeResData(PacketId packet_id, T packet_body);
	static Session* GetSession(uint32_t session_id);
};