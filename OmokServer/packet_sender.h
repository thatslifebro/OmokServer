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

	void NtfRoomUserEnter(std::vector<uint32_t> room_session_ids, Session* session);

	void NtfRoomUserList(Session* session, std::vector<uint32_t> room_session_ids);

	void ResRoomLeave(Session* session, uint32_t result);

	void NtfRoomUserLeave(std::vector<uint32_t> room_session_ids, Session* session);

	void ResRoomChat(Session* session, uint32_t result, std::string chat);

	void NtfRoomChat(std::vector<uint32_t> room_session_ids, Session* session, std::string chat);

	void ResMatch(Session* session);

	void NtfMatched(Session* session, Session* opponent_session);

	void ResReadyOmok(Session* session);

	void NtfStartOmok(Session* session, Session* opponent_session);

	void ResPutMok(Session* session, uint32_t result);

	void NtfPutMok(Session* session, uint32_t x, uint32_t y);

	void NtfGameOver(Session* session, uint32_t result);

private:
	template <typename T>
	std::tuple<std::shared_ptr<char[]>, uint16_t> MakeResData(PacketId packet_id, T packet_body);
};