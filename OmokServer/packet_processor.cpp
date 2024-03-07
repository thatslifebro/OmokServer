#include "packet_processor.h"

void PacketProcessor::Init()
{
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqLogin), [&](Packet packet) { ReqLoginHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqRoomEnter), [&](Packet packet) { ReqRoomEnterHandler(packet); }));
	//todo: 패킷에 대한 핸들러 등록
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
	std::print("받은 메시지 : reqLogin.userid = {}, pw = {}\n", req_login.userid(), req_login.pw());
	
	auto session = GetSession(packet.session_id_);
	if(session == nullptr)
	{
		return;
	}

	//로그인 
	uint32_t result = -1;

	if (user_auth_map[req_login.userid()] == req_login.pw())
	{
		session->Login(req_login.userid());
		result = 0;
		std::print("SessionId : {}, UserId : {} 유저가 로그인함.\n", session->session_id_, session->user_id_);
	}

	//로그인 결과 전송
	PacketSender::ResLogin(session, result);
}

void PacketProcessor::ReqRoomEnterHandler(Packet packet)
{
	OmokPacket::ReqRoomEnter req_room_enter;
	req_room_enter.ParseFromArray(packet.packet_body_, packet.packet_size_);
	std::print("받은 메시지 : reqRoomEnter.room_number = {}\n", req_room_enter.roomid());

	auto session = GetSession(packet.session_id_);
	if (session == nullptr)
	{
		return;
	}

	//방 입장 처리
	uint32_t result = -1;

	if (session->is_logged_in_ == true && session->session_room_id_ == 0)
	{
		auto success = RoomManager::AddSession(session->session_id_, req_room_enter.roomid());
		if (success == true)
		{
			result = 0;
			session->session_room_id_ = req_room_enter.roomid();
			std::print("SessionId : {}, UserId : {} 가 방 {}번에 입장함.\n", session->session_id_, session->user_id_, session->session_room_id_);
		}
	}

	//방 입장 결과 전송
	PacketSender::ResRoomEnter(session, result);
	
	if (result == 0)
	{
		auto room_session_ids = RoomManager::GetSessionList(session->session_room_id_);
		PacketSender::NtfRoomUserList(session, room_session_ids);
		PacketSender::BroadcastRoomUserEnter(room_session_ids, session);
	}

}


Session* PacketProcessor::GetSession(uint32_t session_id)
{
	SessionManager session_manager;
	if (session_manager.IsSessionExist(session_id) == false)
	{
		std::print("Session is not exist\n");
		return nullptr;
	}

	return session_manager.GetSession(session_id);
}