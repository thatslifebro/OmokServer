#include "packet_sender.h"

void PacketSender::ResLogin(uint32_t session_id, uint32_t result)
{
	OmokPacket::ResLogin res_login;
	res_login.set_result(result);

	auto [res_data, res_length] = MakeResData(PacketId::ResLogin, res_login);

	SendPacket(session_id, res_data, res_length);
}

void PacketSender::ResRoomEnter(uint32_t session_id, std::string user_id, uint32_t result)
{
	OmokPacket::UserInfo* userinfo = new OmokPacket::UserInfo();
	userinfo->set_sessionid(session_id);
	userinfo->set_userid(user_id);

	OmokPacket::ResRoomEnter res_room_enter;
	res_room_enter.set_result(result);
	res_room_enter.set_allocated_userinfo(userinfo);

	auto [res_data, res_length] = MakeResData(PacketId::ResRoomEnter, res_room_enter);

	SendPacket(session_id, res_data, res_length);
}

void PacketSender::NtfNewRoomAdmin(std::unordered_set<uint32_t> room_session_ids, uint32_t admin_session_id, std::string admin_user_id)
{
	OmokPacket::UserInfo* userinfo = new OmokPacket::UserInfo();
	userinfo->set_sessionid(admin_session_id);
	userinfo->set_userid(admin_user_id);

	OmokPacket::NtfNewRoomAdmin ntf_new_room_admin;
	ntf_new_room_admin.set_allocated_userinfo(userinfo);

	auto [res_data, res_length] = MakeResData(PacketId::NtfNewRoomAdmin, ntf_new_room_admin);

	for (auto session_id : room_session_ids)
	{
		if(session_id == admin_session_id)
		{
			continue;
		}

		SendPacket(session_id, res_data, res_length);
	}
}

void PacketSender::ResRoomAdmin(uint32_t session_id, uint32_t admin_session_id, std::string admin_user_id)
{
	OmokPacket::UserInfo* userinfo = new OmokPacket::UserInfo();
	userinfo->set_sessionid(admin_session_id);
	userinfo->set_userid(admin_user_id);

	OmokPacket::NtfNewRoomAdmin ntf_new_room_admin;
	ntf_new_room_admin.set_allocated_userinfo(userinfo);

	auto [res_data, res_length] = MakeResData(PacketId::NtfNewRoomAdmin, ntf_new_room_admin);

	SendPacket(session_id, res_data, res_length);

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

	// 전송 데이터 생성
	auto [res_data, res_length] = MakeResData(PacketId::NtfRoomUserList, res_room_user_list);

	// 전송
	SendPacket(session_id, res_data, res_length);
}

void PacketSender::NtfRoomUserEnter(std::unordered_set<uint32_t> room_session_ids, uint32_t session_id, std::string user_id)
{
	OmokPacket::UserInfo* userinfo = new OmokPacket::UserInfo();
	userinfo->set_sessionid(session_id);
	userinfo->set_userid(user_id);

	OmokPacket::NtfRoomNewUser ntf_room_new_user;
	ntf_room_new_user.set_allocated_userinfo(userinfo);

	auto [res_data, res_length] = MakeResData(PacketId::NtfRoomNewUser, ntf_room_new_user);

	for (auto other_session_id : room_session_ids)
	{
		//자신에겐 보내지 않음
		if (other_session_id == session_id)
		{
			continue;
		}

		SendPacket(other_session_id, res_data, res_length);
	}
}

void PacketSender::ResYouAreRoomAdmin(uint32_t session_id)
{
	OmokPacket::ResYouAreRoomAdmin res_you_are_room_admin;

	auto [res_data, res_length] = MakeResData(PacketId::ResYouAreRoomAdmin, res_you_are_room_admin);

	SendPacket(session_id, res_data, res_length);
}

void PacketSender::ResRoomLeave(uint32_t session_id, uint32_t result)
{
	OmokPacket::ResRoomLeave res_room_leave;
	res_room_leave.set_result(result);

	auto [res_data, res_length] = MakeResData(PacketId::ResRoomLeave, res_room_leave);

	SendPacket(session_id, res_data, res_length);
}

void PacketSender::NtfRoomUserLeave(std::unordered_set<uint32_t> room_session_ids, uint32_t session_id, std::string user_id)
{
	OmokPacket::UserInfo* userinfo = new OmokPacket::UserInfo();
	userinfo->set_sessionid(session_id);
	userinfo->set_userid(user_id);

	OmokPacket::NtfRoomLeaveUser ntf_room_leave_user;
	ntf_room_leave_user.set_allocated_userinfo(userinfo);

	auto [res_data, res_length] = MakeResData(PacketId::NtfRoomLeaveUser, ntf_room_leave_user);

	for (auto other_session_id : room_session_ids)
	{
		if (other_session_id == session_id)
		{
			continue;
		}

		SendPacket(other_session_id, res_data, res_length);
	}
}

void PacketSender::ResRoomChat(uint32_t session_id, uint32_t result, std::string chat)
{
	OmokPacket::ResRoomChat res_room_chat;
	res_room_chat.set_result(result);
	res_room_chat.set_chat(chat);

	auto [res_data, res_length] = MakeResData(PacketId::ResRoomChat, res_room_chat);

	SendPacket(session_id, res_data, res_length);
}

void PacketSender::NtfRoomChat(std::unordered_set<uint32_t> room_session_ids, uint32_t session_id, std::string user_id, std::string chat)
{
	OmokPacket::UserInfo* userinfo = new OmokPacket::UserInfo();
	userinfo->set_sessionid(session_id);
	userinfo->set_userid(user_id);

	OmokPacket::NtfRoomChat ntf_room_chat;
	ntf_room_chat.set_allocated_userinfo(userinfo);
	ntf_room_chat.set_chat(chat);

	auto [res_data, res_length] = MakeResData(PacketId::NtfRoomChat, ntf_room_chat);

	for (auto other_session_id : room_session_ids)
	{
		if (other_session_id == session_id)
		{
			continue;
		}

		SendPacket(other_session_id, res_data, res_length);
	}
}

void PacketSender::ResMatch(uint32_t admin_session_id, uint32_t result)
{
	OmokPacket::ResMatch res_match;
	res_match.set_result(result);

	auto [res_data, res_length] = MakeResData(PacketId::ResMatch, res_match);

	SendPacket(admin_session_id, res_data, res_length);
}

void PacketSender::NtfMatchReq(uint32_t opponent_session_id, uint32_t admin_session_id, std::string admin_user_id)
{
	OmokPacket::UserInfo* userinfo = new OmokPacket::UserInfo();
	userinfo->set_sessionid(opponent_session_id);
	userinfo->set_userid(admin_user_id);

	OmokPacket::NtfMatchReq ntf_match_req;
	ntf_match_req.set_allocated_userinfo(userinfo);

	auto [res_data, res_length] = MakeResData(PacketId::NtfMatchReq, ntf_match_req);

	SendPacket(opponent_session_id, res_data, res_length);
}

void PacketSender::ResReadyOmok(uint32_t session_id)
{
	OmokPacket::ResReadyOmok res_ready_omok;

	auto [res_data, res_length] = MakeResData(PacketId::ResReadyOmok, res_ready_omok);

	SendPacket(session_id, res_data, res_length);
}

void PacketSender::NtfStartOmok(uint32_t balck_session_id, std::string black_user_id, uint32_t white_session_id, std::string white_user_id)
{
	// 흑백 정보 및 게임 참여자 ID 전파
	OmokPacket::NtfStartOmok ntf_start_omok_black;
	OmokPacket::NtfStartOmok ntf_start_omok_white;

	ntf_start_omok_black.set_black(true);
	ntf_start_omok_white.set_black(false);

	ntf_start_omok_black.set_myid(black_user_id);
	ntf_start_omok_black.set_opponentid(white_user_id);
	ntf_start_omok_white.set_myid(white_user_id);
	ntf_start_omok_white.set_opponentid(black_user_id);

	auto [res_data_black, res_length_black] = MakeResData(PacketId::NtfStartOmok, ntf_start_omok_black);
	auto [res_data_white, res_length_white] = MakeResData(PacketId::NtfStartOmok, ntf_start_omok_white);

	SendPacket(balck_session_id, res_data_black, res_length_black);
	SendPacket(white_session_id, res_data_white, res_length_white);
}

void PacketSender::NtfStartOmokView(std::unordered_set<uint32_t> room_session_ids, uint32_t black_session_id, std::string balck_user_id, uint32_t white_session_id, std::string white_user_id)
{
	OmokPacket::NtfStartOmokView ntf_start_omok_view;
	ntf_start_omok_view.set_blackid(balck_user_id);
	ntf_start_omok_view.set_whiteid(white_user_id);

	auto [res_data, res_length] = MakeResData(PacketId::NtfStartOmokView, ntf_start_omok_view);
	for (auto session_id : room_session_ids)
	{
		if (session_id == black_session_id || session_id == white_session_id)
		{
			continue;
		}	

		SendPacket(session_id, res_data, res_length);
	}
}

void PacketSender::ResPutMok(uint32_t session_id, uint32_t result)
{
	OmokPacket::ResPutMok res_put_mok;
	res_put_mok.set_result(result);

	auto [res_data, res_length] = MakeResData(PacketId::ResPutMok, res_put_mok);

	SendPacket(session_id, res_data, res_length);
}

void PacketSender::NtfPutMok(std::unordered_set<uint32_t> room_session_ids, uint32_t session_id, uint32_t x, uint32_t y)
{
	OmokPacket::NtfPutMok ntf_put_mok;
	ntf_put_mok.set_x(x);
	ntf_put_mok.set_y(y);

	auto [res_data, res_length] = MakeResData(PacketId::NtfPutMok, ntf_put_mok);

	for (auto other_session_id : room_session_ids)
	{
		if (other_session_id == session_id)
		{
			continue;
		}

		SendPacket(other_session_id, res_data, res_length);
	}
}

void PacketSender::NtfGameOver(uint32_t session_id, uint32_t result)
{
	OmokPacket::NtfEndOmok ntf_end_omok;
	ntf_end_omok.set_status(result);

	auto [res_data, res_length] = MakeResData(PacketId::NtfEndOmok, ntf_end_omok);

	SendPacket(session_id, res_data, res_length);
}

void PacketSender::NtfGameOverView(std::unordered_set<uint32_t> room_session_ids, uint32_t winner_id, uint32_t loser_id, uint32_t result)
{
	OmokPacket::NtfEndOmok ntf_end_omok;
	ntf_end_omok.set_status(result);

	auto [res_data, res_length] = MakeResData(PacketId::NtfEndOmok, ntf_end_omok);

	for (auto session_id : room_session_ids)
	{
		if (session_id == winner_id || session_id == loser_id)
		{
			continue;
		}

		SendPacket(session_id, res_data, res_length);
	}
}

void PacketSender::NtfMatchTimeout(uint32_t session_id)
{
	OmokPacket::NtfMatchTimeout ntf_match_timeout;

	auto [res_data, res_length] = MakeResData(PacketId::NtfMatchTimeout, ntf_match_timeout);

	SendPacket(session_id, res_data, res_length);
}

void PacketSender::NtfReadyTimeout(uint32_t session_id)
{
	OmokPacket::NtfReadyTimeout ntf_ready_timeout;

	auto [res_data, res_length] = MakeResData(PacketId::NtfReadyTimeout, ntf_ready_timeout);

	SendPacket(session_id, res_data, res_length);
}

void PacketSender::NtfPutMokTimeout(std::unordered_set<uint32_t> room_session_ids)
{
	OmokPacket::NtfPutMokTimeout ntf_put_mok_timeout;

	auto [res_data, res_length] = MakeResData(PacketId::NtfPutMokTimeout, ntf_put_mok_timeout);
	for (auto session_id : room_session_ids)
	{
		SendPacket(session_id, res_data, res_length);
	}
}

template <typename T>
std::tuple<std::shared_ptr<char[]>, uint16_t> PacketSender::MakeResData(PacketId packet_id, T packet_body)
{
	Packet res_login_packet;

	res_login_packet.packet_id_ = static_cast<uint16_t>(packet_id);
	res_login_packet.packet_size_ = packet_body.ByteSizeLong() + PacketHeader::header_size_;
	packet_body.SerializeToArray(res_login_packet.packet_body_, packet_body.ByteSizeLong());

	return res_login_packet.ToByteArray();
}