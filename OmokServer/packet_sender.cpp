#include "packet_sender.h"

void PacketSender::ResLogin(Session* session, uint32_t result)
{
	OmokPacket::ResLogin res_login;
	res_login.set_result(result);

	auto [res_data, res_length] = MakeResData(PacketId::ResLogin, res_login);

	session->SendPacket(res_data, res_length);

	delete res_data;
}

void PacketSender::ResRoomEnter(Session* session, uint32_t result)
{
	OmokPacket::ResRoomEnter res_room_enter;
	res_room_enter.set_result(result);

	auto [res_data, res_length] = MakeResData(PacketId::ResRoomEnter, res_room_enter);

	session->SendPacket(res_data, res_length);

	delete res_data;
}


void PacketSender::NtfRoomUserList(Session* session, std::vector<uint32_t> room_session_ids)
{
	// 傈价 菩哦 积己
	OmokPacket::NtfRoomUserList ntf_room_user_list;
	for (auto session_id : room_session_ids)
	{
		auto session = GetSession(session_id);
		if (session == nullptr)
		{
			continue;
		}
		auto user = ntf_room_user_list.add_user();
		user->set_sessionid(session->session_id_);
		user->set_userid(session->user_id_);
	}

	// 傈价 单捞磐 积己
	auto [res_data, res_length] = MakeResData(PacketId::NtfRoomUserList, ntf_room_user_list);

	// 傈价
	session->SendPacket(res_data, res_length);

	delete res_data;
}

void PacketSender::BroadcastRoomUserEnter(std::vector<uint32_t> room_session_ids, Session* session)
{
	for (auto session_id : room_session_ids)
	{
		if (session_id == session->session_id_)
		{
			continue;
		}

		auto other_session = GetSession(session_id);
		if (other_session == nullptr)
		{
			continue;
		}

		// 傈价 菩哦 积己
		OmokPacket::NtfRoomNewUser ntf_room_new_user;
		ntf_room_new_user.set_allocated_user(new OmokPacket::User());
		ntf_room_new_user.mutable_user()->set_sessionid(session->session_id_);
		ntf_room_new_user.mutable_user()->set_userid(session->user_id_);

		// 傈价 单捞磐 积己
		auto [res_data, res_length] = MakeResData(PacketId::NtfRoomNewUser, ntf_room_new_user);

		// 傈价
		other_session->SendPacket(res_data, res_length);

		delete res_data;
	}
}

template <typename T>
std::tuple<char*, uint16_t> PacketSender::MakeResData(PacketId packet_id, T packet_body)
{
	Packet res_login_packet;

	res_login_packet.packet_id_ = static_cast<uint16_t>(packet_id);
	res_login_packet.packet_size_ = packet_body.ByteSizeLong() + PacketHeader::header_size_;
	packet_body.SerializeToArray(res_login_packet.packet_body_, packet_body.ByteSizeLong());

	return res_login_packet.ToByteArray();
}

Session* PacketSender::GetSession(uint32_t session_id)
{
	SessionManager session_manager;
	if (session_manager.IsSessionExist(session_id) == false)
	{
		std::print("Session is not exist\n");
		return nullptr;
	}

	return session_manager.GetSession(session_id);
}