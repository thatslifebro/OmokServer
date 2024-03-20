#include "packet_sender.h"

void PacketSender::ResLogin(uint32_t session_id, uint32_t result)
{
	OmokPacket::ResLogin res_login;
	res_login.set_result(result);

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::ResLogin, res_login);

	SendPacket_(session_id, res_data, res_length);
}

void PacketSender::ResRoomEnter(uint32_t session_id, std::string user_id, uint32_t result)
{
	OmokPacket::UserInfo* userinfo = new OmokPacket::UserInfo();
	userinfo->set_sessionid(session_id);
	userinfo->set_userid(user_id);

	OmokPacket::ResRoomEnter res_room_enter;
	res_room_enter.set_result(result);
	res_room_enter.set_allocated_userinfo(userinfo);

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::ResRoomEnter, res_room_enter);

	SendPacket_(session_id, res_data, res_length);
}

void PacketSender::NtfNewRoomAdmin(std::unordered_set<uint32_t> room_session_ids, uint32_t admin_session_id, std::string admin_user_id)
{
	OmokPacket::UserInfo* userinfo = new OmokPacket::UserInfo();
	userinfo->set_sessionid(admin_session_id);
	userinfo->set_userid(admin_user_id);

	OmokPacket::NtfNewRoomAdmin ntf_new_room_admin;
	ntf_new_room_admin.set_allocated_userinfo(userinfo);

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::NtfNewRoomAdmin, ntf_new_room_admin);

	for (auto session_id : room_session_ids)
	{
		if(session_id == admin_session_id)
		{
			continue;
		}

		SendPacket_(session_id, res_data, res_length);
	}
}

void PacketSender::ResRoomAdmin(uint32_t session_id, uint32_t admin_session_id, std::string admin_user_id)
{
	OmokPacket::UserInfo* userinfo = new OmokPacket::UserInfo();
	userinfo->set_sessionid(admin_session_id);
	userinfo->set_userid(admin_user_id);

	OmokPacket::NtfNewRoomAdmin ntf_new_room_admin;
	ntf_new_room_admin.set_allocated_userinfo(userinfo);

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::NtfNewRoomAdmin, ntf_new_room_admin);

	SendPacket_(session_id, res_data, res_length);

}

void PacketSender::ResRoomUserList(uint32_t session_id, std::vector<std::pair<uint32_t,std::string>> user_info_vec)
{
	OmokPacket::ResRoomUserList res_room_user_list;
	for (auto user_info : user_info_vec)
	{
		if(user_info.first == session_id)
		{
			continue;
		}

		auto userinfo = res_room_user_list.add_userinfo();
		userinfo->set_sessionid(user_info.first);
		userinfo->set_userid(user_info.second);
	}

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::NtfRoomUserList, res_room_user_list);

	SendPacket_(session_id, res_data, res_length);
}

void PacketSender::NtfRoomUserEnter(std::unordered_set<uint32_t> room_session_ids, uint32_t session_id, std::string user_id)
{
	OmokPacket::UserInfo* userinfo = new OmokPacket::UserInfo();
	userinfo->set_sessionid(session_id);
	userinfo->set_userid(user_id);

	OmokPacket::NtfRoomNewUser ntf_room_new_user;
	ntf_room_new_user.set_allocated_userinfo(userinfo);

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::NtfRoomNewUser, ntf_room_new_user);

	for (auto other_session_id : room_session_ids)
	{
		if (other_session_id == session_id)
		{
			continue;
		}

		SendPacket_(other_session_id, res_data, res_length);
	}
}

void PacketSender::ResYouAreRoomAdmin(uint32_t session_id)
{
	OmokPacket::ResYouAreRoomAdmin res_you_are_room_admin;

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::ResYouAreRoomAdmin, res_you_are_room_admin);

	SendPacket_(session_id, res_data, res_length);
}

void PacketSender::ResRoomLeave(uint32_t session_id, uint32_t result)
{
	OmokPacket::ResRoomLeave res_room_leave;
	res_room_leave.set_result(result);

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::ResRoomLeave, res_room_leave);

	SendPacket_(session_id, res_data, res_length);
}

void PacketSender::NtfRoomUserLeave(std::unordered_set<uint32_t> room_session_ids, uint32_t session_id, std::string user_id)
{
	OmokPacket::UserInfo* userinfo = new OmokPacket::UserInfo();
	userinfo->set_sessionid(session_id);
	userinfo->set_userid(user_id);

	OmokPacket::NtfRoomLeaveUser ntf_room_leave_user;
	ntf_room_leave_user.set_allocated_userinfo(userinfo);

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::NtfRoomLeaveUser, ntf_room_leave_user);

	for (auto other_session_id : room_session_ids)
	{
		if (other_session_id == session_id)
		{
			continue;
		}

		SendPacket_(other_session_id, res_data, res_length);
	}
}

void PacketSender::ResRoomChat(uint32_t session_id, uint32_t result, std::string chat)
{
	OmokPacket::ResRoomChat res_room_chat;
	res_room_chat.set_result(result);
	res_room_chat.set_chat(chat);

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::ResRoomChat, res_room_chat);

	SendPacket_(session_id, res_data, res_length);
}

void PacketSender::NtfRoomChat(std::unordered_set<uint32_t> room_session_ids, uint32_t session_id, std::string user_id, std::string chat)
{
	OmokPacket::UserInfo* userinfo = new OmokPacket::UserInfo();
	userinfo->set_sessionid(session_id);
	userinfo->set_userid(user_id);

	OmokPacket::NtfRoomChat ntf_room_chat;
	ntf_room_chat.set_allocated_userinfo(userinfo);
	ntf_room_chat.set_chat(chat);

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::NtfRoomChat, ntf_room_chat);

	for (auto other_session_id : room_session_ids)
	{
		if (other_session_id == session_id)
		{
			continue;
		}

		SendPacket_(other_session_id, res_data, res_length);
	}
}

void PacketSender::ResMatch(uint32_t admin_session_id, uint32_t result)
{
	OmokPacket::ResMatch res_match;
	res_match.set_result(result);

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::ResMatch, res_match);

	SendPacket_(admin_session_id, res_data, res_length);
}

void PacketSender::NtfMatchReq(uint32_t opponent_session_id, uint32_t admin_session_id, std::string admin_user_id)
{
	OmokPacket::UserInfo* userinfo = new OmokPacket::UserInfo();
	userinfo->set_sessionid(opponent_session_id);
	userinfo->set_userid(admin_user_id);

	OmokPacket::NtfMatchReq ntf_match_req;
	ntf_match_req.set_allocated_userinfo(userinfo);

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::NtfMatchReq, ntf_match_req);

	SendPacket_(opponent_session_id, res_data, res_length);
}

void PacketSender::ResReadyOmok(uint32_t session_id)
{
	OmokPacket::ResReadyOmok res_ready_omok;

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::ResReadyOmok, res_ready_omok);

	SendPacket_(session_id, res_data, res_length);
}

void PacketSender::NtfStartOmok(uint32_t balck_session_id, std::string black_user_id, uint32_t white_session_id, std::string white_user_id)
{
	OmokPacket::NtfStartOmok ntf_start_omok_black;
	OmokPacket::NtfStartOmok ntf_start_omok_white;

	ntf_start_omok_black.set_black(true);
	ntf_start_omok_white.set_black(false);

	ntf_start_omok_black.set_myid(black_user_id);
	ntf_start_omok_black.set_opponentid(white_user_id);
	ntf_start_omok_white.set_myid(white_user_id);
	ntf_start_omok_white.set_opponentid(black_user_id);

	Packet packet1;
	Packet packet2;
	auto [res_data_black, res_length_black] = MakeResData(packet1, PacketId::NtfStartOmok, ntf_start_omok_black);
	auto [res_data_white, res_length_white] = MakeResData(packet2, PacketId::NtfStartOmok, ntf_start_omok_white);

	SendPacket_(balck_session_id, res_data_black, res_length_black);
	SendPacket_(white_session_id, res_data_white, res_length_white);
}

void PacketSender::NtfStartOmokView(std::unordered_set<uint32_t> room_session_ids, uint32_t black_session_id, std::string balck_user_id, uint32_t white_session_id, std::string white_user_id)
{
	OmokPacket::NtfStartOmokView ntf_start_omok_view;
	ntf_start_omok_view.set_blackid(balck_user_id);
	ntf_start_omok_view.set_whiteid(white_user_id);

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::NtfStartOmokView, ntf_start_omok_view);
	for (auto session_id : room_session_ids)
	{
		if (session_id == black_session_id || session_id == white_session_id)
		{
			continue;
		}	

		SendPacket_(session_id, res_data, res_length);
	}
}

void PacketSender::ResPutMok(uint32_t session_id, uint32_t result)
{
	OmokPacket::ResPutMok res_put_mok;
	res_put_mok.set_result(result);

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::ResPutMok, res_put_mok);

	SendPacket_(session_id, res_data, res_length);
}

void PacketSender::NtfPutMok(std::unordered_set<uint32_t> room_session_ids, uint32_t session_id, uint32_t x, uint32_t y)
{
	OmokPacket::NtfPutMok ntf_put_mok;
	ntf_put_mok.set_x(x);
	ntf_put_mok.set_y(y);

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::NtfPutMok, ntf_put_mok);

	for (auto other_session_id : room_session_ids)
	{
		if (other_session_id == session_id)
		{
			continue;
		}

		SendPacket_(other_session_id, res_data, res_length);
	}
}

void PacketSender::NtfGameOver(uint32_t session_id, uint32_t result)
{
	OmokPacket::NtfEndOmok ntf_end_omok;
	ntf_end_omok.set_status(result);

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::NtfEndOmok, ntf_end_omok);

	SendPacket_(session_id, res_data, res_length);
}

void PacketSender::NtfGameOverView(std::unordered_set<uint32_t> room_session_ids, uint32_t winner_id, uint32_t loser_id)
{
	OmokPacket::NtfEndOmok ntf_end_omok;

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::NtfEndOmok, ntf_end_omok);

	for (auto session_id : room_session_ids)
	{
		if (session_id == winner_id || session_id == loser_id)
		{
			continue;
		}

		SendPacket_(session_id, res_data, res_length);
	}
}

void PacketSender::NtfMatchTimeout(uint32_t session_id)
{
	OmokPacket::NtfMatchTimeout ntf_match_timeout;

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::NtfMatchTimeout, ntf_match_timeout);

	SendPacket_(session_id, res_data, res_length);
}

void PacketSender::NtfReadyTimeout(uint32_t session_id)
{
	OmokPacket::NtfReadyTimeout ntf_ready_timeout;

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::NtfReadyTimeout, ntf_ready_timeout);

	SendPacket_(session_id, res_data, res_length);
}

void PacketSender::NtfPutMokTimeout(std::unordered_set<uint32_t> room_session_ids)
{
	OmokPacket::NtfPutMokTimeout ntf_put_mok_timeout;

	Packet packet;
	auto [res_data, res_length] = MakeResData(packet, PacketId::NtfPutMokTimeout, ntf_put_mok_timeout);
	for (auto session_id : room_session_ids)
	{
		SendPacket_(session_id, res_data, res_length);
	}
}

template <typename T>
std::tuple<char*, uint16_t> PacketSender::MakeResData(Packet packet, PacketId packet_id, T packet_body)
{
	auto body_size = packet_body.ByteSizeLong();

	packet.SetPacketId(static_cast<uint16_t>(packet_id));
	packet.SetPacketSize(body_size + PacketHeader::HEADER_SIZE);
	packet_body.SerializeToArray(packet.GetPacketBody(), body_size);

	return packet.GetByteArray();
}