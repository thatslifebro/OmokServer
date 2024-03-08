#pragma once
#include "protobuf/OmokPacket.pb.h"
#include "session.h"
#include "packet_id.h"

class PacketSender
{
public:
	SessionManager session_manager_;

	void ResLogin(Session* session, uint32_t result);

	void ResRoomEnter(Session* session, uint32_t result);

	void BroadcastRoomUserEnter(std::vector<uint32_t> room_session_ids, Session* session);

	void NtfRoomUserList(Session* session, std::vector<uint32_t> room_session_ids);

	void ResRoomLeave(Session* session, uint32_t result);

	void BroadcastRoomUserLeave(std::vector<uint32_t> room_session_ids, Session* session);

	void ResRoomChat(Session* session, uint32_t result, std::string chat);

	void BroadcastRoomChat(std::vector<uint32_t> room_session_ids, Session* session, std::string chat);

	void ResMatch(Session* session);

	void NtfMatched(Session* session, Session* oponent_session);

private:
	template <typename T>
	std::tuple<char*, uint16_t> MakeResData(PacketId packet_id, T packet_body);
};