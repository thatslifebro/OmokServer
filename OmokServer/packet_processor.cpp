#include "packet_processor.h"

void PacketProcessor::Init()
{
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqLogin), [&](Packet packet) { ReqLoginHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqRoomEnter), [&](Packet packet) { ReqRoomEnterHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqRoomLeave), [&](Packet packet) { ReqRoomLeaveHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqRoomChat), [&](Packet packet) { ReqRoomChatHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqMatch), [&](Packet packet) { ReqMatchHandler(packet); }));
	//todo: ��Ŷ�� ���� �ڵ鷯 ���
}

bool PacketProcessor::ProcessPacket()
{
	PacketQueue packet_queue;
	auto packet = packet_queue.PopAndGetPacket();

	if (packet.packet_size_ <= 0)
	{
		return false;
	}

	packet_handler_map_[packet.packet_id_](packet);
	return true;
}

void PacketProcessor::ReqLoginHandler(Packet packet)
{
	OmokPacket::ReqLogin req_login;
	req_login.ParseFromArray(packet.packet_body_, packet.packet_size_);
	
	auto session = session_manager_.GetSession(packet.session_id_);
	if(session == nullptr)
	{
		return;
	}

	std::print("���� �޽��� : reqLogin.userid = {}, pw = {}\n", req_login.userid(), req_login.pw());

	//�α��� 
	uint32_t result = -1;

	if (user_auth_map[req_login.userid()] == req_login.pw())
	{
		session->Login(req_login.userid());
		result = 0;
		std::print("SessionId : {}, UserId : {} ������ �α�����.\n", session->session_id_, session->user_id_);
	}

	//�α��� ��� ����
	packet_sender_.ResLogin(session, result);
}

void PacketProcessor::ReqRoomEnterHandler(Packet packet)
{
	OmokPacket::ReqRoomEnter req_room_enter;
	req_room_enter.ParseFromArray(packet.packet_body_, packet.packet_size_);
	
	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		return;
	}

	std::print("���� �޽��� : user_id = {}, reqRoomEnter.room_number = {}\n", session->user_id_ ,req_room_enter.roomid());

	//�� ���� ó��
	uint32_t result = -1;

	if (session->is_logged_in_ == true && session->session_room_id_ == 0)
	{
		auto success = room_manager_.AddSession(session->session_id_, req_room_enter.roomid());
		if (success == true)
		{
			result = 0;
			session->session_room_id_ = req_room_enter.roomid();
			std::print("SessionId : {}, UserId : {} �� �� {}���� ������.\n", session->session_id_, session->user_id_, session->session_room_id_);
		}
	}

	//�� ���� ��� ����
	packet_sender_.ResRoomEnter(session, result);
	
	if (result == 0)
	{
		auto room_session_ids = room_manager_.GetSessionList(session->session_room_id_);
		packet_sender_.NtfRoomUserList(session, room_session_ids);
		packet_sender_.BroadcastRoomUserEnter(room_session_ids, session);
	}
}

void PacketProcessor::ReqRoomLeaveHandler(Packet packet)
{
	OmokPacket::ReqRoomLeave req_room_leave;
	req_room_leave.ParseFromArray(packet.packet_body_, packet.packet_size_);

	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		return;
	}

	std::print("���� �޽��� : user_id = {}, reqRoomLeave\n", session->user_id_);

	auto room_id = session->session_room_id_;

	//�� ���� ó��
	uint32_t result = -1;

	if (session->is_logged_in_ == true && room_id != 0)
	{
		auto success = room_manager_.RemoveSession(session->session_id_, room_id);
		if (success == true)
		{
			result = 0;
			session->session_room_id_ = 0;
			std::print("SessionId : {}, UserId : {} �� �濡�� ����.\n", session->session_id_, session->user_id_);
		}
	}

	//�� ���� ��� ����
	packet_sender_.ResRoomLeave(session, result);

	if (result == 0)
	{
		auto room_session_ids = room_manager_.GetSessionList(room_id);
		packet_sender_.BroadcastRoomUserLeave(room_session_ids, session);
	}
}

void PacketProcessor::ReqRoomChatHandler(Packet packet)
{
	OmokPacket::ReqRoomChat req_room_chat;
	req_room_chat.ParseFromArray(packet.packet_body_, packet.packet_size_);

	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		return;
	}

	std::print("���� �޽��� : user_id = {}, reqRoomChat\n", session->user_id_);

	//ä�� ó��
	uint32_t result = -1;

	if (session->is_logged_in_ == true && session->session_room_id_ != 0)
	{
		result = 0;
		std::print("SessionId : {}, UserId : {} �� ä���� ����.\n", session->session_id_, session->user_id_);
	}

	//�ڽſ��� ä�� ��� ����
	packet_sender_.ResRoomChat(session, result, req_room_chat.chat());

	//�ٸ� �������� ä�� ����
	if (result == 0)
	{
		auto room_session_ids = room_manager_.GetSessionList(session->session_room_id_);
		packet_sender_.BroadcastRoomChat(room_session_ids, session, req_room_chat.chat());
	}
}

void PacketProcessor::ReqMatchHandler(Packet packet)
{
	// body ���� ��Ŷ

	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		return;
	}

	std::print("���� �޽��� : user_id = {}, reqMatch\n", session->user_id_);

	//��Ī ó��
	if(session->is_matching_ == true || session->game_room_id_!=0)
	{
		return;
	}

	auto oponent_id = game_room_manager_.Match(session);

	//��Ī ��� ����
	packet_sender_.ResMatch(session);

	if (oponent_id != 0)
	{
		auto oponent_session = session_manager_.GetSession(oponent_id);
		packet_sender_.NtfMatched(session, oponent_session);
	}
}