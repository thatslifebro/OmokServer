#include "packet_sender.h"

void PacketSender::ResLogin(Session* session, uint32_t result)
{
	OmokPacket::ResLogin res_login;
	res_login.set_result(result);

	auto [res_data, res_length] = MakeResData(PacketId::ResLogin, res_login);

	session->SendPacket(res_data, res_length);
}

void PacketSender::ResRoomEnter(Session* session, uint32_t result)
{
	OmokPacket::ResRoomEnter res_room_enter;
	res_room_enter.set_result(result);

	auto [res_data, res_length] = MakeResData(PacketId::ResRoomEnter, res_room_enter);

	session->SendPacket(res_data, res_length);
}


void PacketSender::NtfRoomUserList(Session* session, std::vector<uint32_t> room_session_ids)
{
	// 전송 패킷 바디 생성
	OmokPacket::NtfRoomUserList ntf_room_user_list;
	for (auto session_id : room_session_ids)
	{
		auto session = session_manager_.GetSession(session_id);
		if (session == nullptr)
		{
			continue;
		}
		ntf_room_user_list.add_userid(session->user_id_);
	}

	// 전송 데이터 생성
	auto [res_data, res_length] = MakeResData(PacketId::NtfRoomUserList, ntf_room_user_list);

	// 전송
	session->SendPacket(res_data, res_length);
}

void PacketSender::NtfRoomUserEnter(std::vector<uint32_t> room_session_ids, Session* session)
{
	OmokPacket::NtfRoomNewUser ntf_room_new_user;
	ntf_room_new_user.set_userid(session->user_id_);

	auto [res_data, res_length] = MakeResData(PacketId::NtfRoomNewUser, ntf_room_new_user);

	for (auto session_id : room_session_ids)
	{
		//자신에겐 보내지 않음
		if (session_id == session->session_id_)
		{
			continue;
		}

		auto other_session = session_manager_.GetSession(session_id);
		if (other_session == nullptr)
		{
			continue;
		}

		other_session->SendPacket(res_data, res_length);
	}
}

void PacketSender::ResRoomLeave(Session* session, uint32_t result)
{
	OmokPacket::ResRoomLeave res_room_leave;
	res_room_leave.set_result(result);

	auto [res_data, res_length] = MakeResData(PacketId::ResRoomLeave, res_room_leave);

	session->SendPacket(res_data, res_length);
}

void PacketSender::NtfRoomUserLeave(std::vector<uint32_t> room_session_ids, Session* session)
{
	OmokPacket::NtfRoomLeaveUser ntf_room_leave_user;
	ntf_room_leave_user.set_userid(session->user_id_);

	auto [res_data, res_length] = MakeResData(PacketId::NtfRoomLeaveUser, ntf_room_leave_user);

	for (auto session_id : room_session_ids)
	{
		if (session_id == session->session_id_)
		{
			continue;
		}

		auto other_session = session_manager_.GetSession(session_id);
		if (other_session == nullptr)
		{
			continue;
		}

		other_session->SendPacket(res_data, res_length);
	}
}

void PacketSender::ResRoomChat(Session* session, uint32_t result, std::string chat)
{
	OmokPacket::ResRoomChat res_room_chat;
	res_room_chat.set_result(result);
	res_room_chat.set_chat(chat);

	auto [res_data, res_length] = MakeResData(PacketId::ResRoomChat, res_room_chat);

	session->SendPacket(res_data, res_length);
}

void PacketSender::NtfRoomChat(std::vector<uint32_t> room_session_ids, Session* session, std::string chat)
{
	OmokPacket::NtfRoomChat ntf_room_chat;
	ntf_room_chat.set_userid(session->user_id_);
	ntf_room_chat.set_chat(chat);

	auto [res_data, res_length] = MakeResData(PacketId::NtfRoomChat, ntf_room_chat);

	for (auto session_id : room_session_ids)
	{
		if (session_id == session->session_id_)
		{
			continue;
		}

		auto other_session = session_manager_.GetSession(session_id);
		if (other_session == nullptr)
		{
			continue;
		}

		other_session->SendPacket(res_data, res_length);
	}
}

void PacketSender::ResMatch(Session* session)
{
	OmokPacket::ResMatch res_match;

	auto [res_data, res_length] = MakeResData(PacketId::ResMatch, res_match);

	session->SendPacket(res_data, res_length);
}

void PacketSender::NtfMatched(Session* session, Session* opponent_session)
{
	// 양쪽 모두 보내기
	OmokPacket::NtfMatched ntf_matched;
	OmokPacket::NtfMatched ntf_matched_opponent;
	ntf_matched.set_userid(opponent_session->user_id_);
	ntf_matched_opponent.set_userid(session->user_id_);

	auto [res_data, res_length] = MakeResData(PacketId::NtfMatched, ntf_matched);
	auto [res_data_opponent, res_length_opponent] = MakeResData(PacketId::NtfMatched, ntf_matched_opponent);

	session->SendPacket(res_data, res_length);
	opponent_session->SendPacket(res_data_opponent, res_length_opponent);
}

void PacketSender::ResReadyOmok(Session* session)
{
	OmokPacket::ResReadyOmok res_ready_omok;

	auto [res_data, res_length] = MakeResData(PacketId::ResReadyOmok, res_ready_omok);

	session->SendPacket(res_data, res_length);
}

void PacketSender::NtfStartOmok(Session* balck_session, Session* white_session)
{
	// 흑백 정보 및 게임 참여자 ID 전파
	OmokPacket::NtfStartOmok ntf_start_omok_black;
	OmokPacket::NtfStartOmok ntf_start_omok_white;

	ntf_start_omok_black.set_black(true);
	ntf_start_omok_white.set_black(false);

	ntf_start_omok_black.set_myid(balck_session->user_id_);
	ntf_start_omok_black.set_opponentid(white_session->user_id_);
	ntf_start_omok_white.set_myid(white_session->user_id_);
	ntf_start_omok_white.set_opponentid(balck_session->user_id_);

	auto [res_data_black, res_length_black] = MakeResData(PacketId::NtfStartOmok, ntf_start_omok_black);
	auto [res_data_white, res_length_white] = MakeResData(PacketId::NtfStartOmok, ntf_start_omok_white);

	balck_session->SendPacket(res_data_black, res_length_black);
	white_session->SendPacket(res_data_white, res_length_white);
}

void PacketSender::ResPutMok(Session* session, uint32_t result)
{
	OmokPacket::ResPutMok res_put_mok;
	res_put_mok.set_result(result);

	auto [res_data, res_length] = MakeResData(PacketId::ResPutMok, res_put_mok);

	session->SendPacket(res_data, res_length);
}

void PacketSender::NtfPutMok(Session* session, uint32_t x, uint32_t y)
{
	OmokPacket::NtfPutMok ntf_put_mok;
	ntf_put_mok.set_x(x);
	ntf_put_mok.set_y(y);

	auto [res_data, res_length] = MakeResData(PacketId::NtfPutMok, ntf_put_mok);

	session->SendPacket(res_data, res_length);
}

void PacketSender::NtfGameOver(Session* session, uint32_t result)
{
	OmokPacket::NtfEndOmok ntf_end_omok;
	ntf_end_omok.set_status(result);

	auto [res_data, res_length] = MakeResData(PacketId::NtfEndOmok, ntf_end_omok);

	session->SendPacket(res_data, res_length);
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