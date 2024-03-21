#pragma once
#include "protobuf/OmokPacket.pb.h"

#include "packet.h"

class PacketSender
{
public:
	void InitSendPacketFunc(std::function<void(uint32_t, char *, uint16_t)> SendPacket) { SendPacket_ = SendPacket; }

	void ResLogin(uint32_t session_id, uint32_t result);

	void ResRoomEnter(uint32_t session_id, std::string user_id, uint32_t result);

	void NtfRoomUserEnter(std::unordered_set<uint32_t> room_session_ids, uint32_t session_id, std::string user_id);

	void ResYouAreRoomAdmin(uint32_t session_id);

	void NtfNewRoomAdmin(std::unordered_set<uint32_t> room_session_ids, uint32_t session_id, std::string user_id);

	void ResRoomAdmin(uint32_t session_id, uint32_t admin_session_id, std::string admin_user_id);

	void ResRoomUserList(uint32_t session_id, std::vector<std::pair<uint32_t, std::string>> user_info_vec);

	void ResRoomLeave(uint32_t session_id, uint32_t result);

	void NtfRoomUserLeave(std::unordered_set<uint32_t> room_session_ids, uint32_t session_id, std::string user_id);

	void ResRoomChat(uint32_t session_id, uint32_t result, std::string chat);

	void NtfRoomChat(std::unordered_set<uint32_t> room_session_ids, uint32_t session_id, std::string user_id, std::string chat);

	void ResMatch(uint32_t session_id, uint32_t result);

	void NtfMatchReq(uint32_t opponent_session_id, uint32_t admin_session, std::string admin_user_id);

	void ResReadyOmok(uint32_t session_id);

	void NtfStartOmok(uint32_t black_session_id, std::string black_user_id, uint32_t white_session_id, std::string white_user_id);

	void NtfStartOmokView(std::unordered_set<uint32_t> room_session_ids, uint32_t black_session_id, std::string balck_user_id, uint32_t white_session_id, std::string white_user_id);

	void ResPutMok(uint32_t session_id, uint32_t result);

	void NtfPutMok(std::unordered_set<uint32_t> room_session_ids, uint32_t session_id, uint32_t x, uint32_t y);

	void NtfGameOver(uint32_t session_id, uint32_t result);

	void NtfGameOverView(std::unordered_set<uint32_t> room_session_ids, uint32_t winner_id, uint32_t loser_id);

	void NtfMatchTimeout(uint32_t session_id);

	void NtfReadyTimeout(uint32_t session_id);

	void NtfPutMokTimeout(std::unordered_set<uint32_t> room_session_ids);

private:
	std::function<void(uint32_t, char*, uint16_t)> SendPacket_;

	template <typename T>
	std::tuple<char*, uint16_t> MakeResData(Packet packet, PacketId packet_id, T packet_body);
};