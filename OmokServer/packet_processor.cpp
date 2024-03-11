#include "packet_processor.h"

void PacketProcessor::Init()
{
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqLogin), [&](Packet packet) { ReqLoginHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqRoomEnter), [&](Packet packet) { ReqRoomEnterHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqRoomLeave), [&](Packet packet) { ReqRoomLeaveHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqRoomChat), [&](Packet packet) { ReqRoomChatHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqMatch), [&](Packet packet) { ReqMatchHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqReadyOmok), [&](Packet packet) { ReqReadyOmokHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqPutMok), [&](Packet packet) { ReqOmokPutHandler(packet); }));
	//todo: 패킷에 대한 핸들러 등록
}

bool PacketProcessor::ProcessPacket()
{
	auto packet = packet_queue_.PopAndGetPacket();

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

	if(session->is_logged_in_ == true)
	{
		return;
	}

	std::print("받은 메시지 : reqLogin.userid = {}, pw = {}\n", req_login.userid(), req_login.pw());

	//db 쓰레드에 로그인 요청
	db_processor_.AddLoginRequest(session, req_login.userid(), req_login.pw());
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

	std::print("받은 메시지 : user_id = {}, reqRoomEnter.room_number = {}\n", session->user_id_ ,req_room_enter.roomid());

	//방 입장 처리
	uint32_t result = -1;

	if (session->is_logged_in_ == true && session->room_id_ == 0)
	{
		auto success = room_manager_.AddSession(session->session_id_, req_room_enter.roomid());
		if (success == true)
		{
			result = 0;
			session->room_id_ = req_room_enter.roomid();
			std::print("SessionId : {}, UserId : {} 가 방 {}번에 입장함.\n", session->session_id_, session->user_id_, session->room_id_);
		}
	}

	//방 입장 결과 전송
	packet_sender_.ResRoomEnter(session, result);
	
	if (result == 0)
	{
		auto room_session_ids = room_manager_.GetSessionList(session->room_id_);
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

	std::print("받은 메시지 : user_id = {}, reqRoomLeave\n", session->user_id_);

	auto room_id = session->room_id_;

	//방 나감 처리
	uint32_t result = -1;

	if (session->is_logged_in_ == true && room_id != 0)
	{
		auto success = room_manager_.RemoveSession(session->session_id_, room_id);
		if (success == true)
		{
			result = 0;
			session->room_id_ = 0;
			std::print("SessionId : {}, UserId : {} 가 방에서 나감.\n", session->session_id_, session->user_id_);
		}
	}

	//방 나감 결과 전송
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

	std::print("받은 메시지 : user_id = {}, reqRoomChat\n", session->user_id_);

	//채팅 처리
	uint32_t result = -1;

	if (session->is_logged_in_ == true && session->room_id_ != 0)
	{
		result = 0;
		std::print("SessionId : {}, UserId : {} 가 채팅을 보냄.\n", session->session_id_, session->user_id_);
	}

	//자신에게 채팅 결과 전송
	packet_sender_.ResRoomChat(session, result, req_room_chat.chat());

	//다른 유저에게 채팅 전송
	if (result == 0)
	{
		auto room_session_ids = room_manager_.GetSessionList(session->room_id_);
		packet_sender_.BroadcastRoomChat(room_session_ids, session, req_room_chat.chat());
	}
}

void PacketProcessor::ReqMatchHandler(Packet packet)
{
	// body 없는 패킷

	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		return;
	}

	std::print("받은 메시지 : user_id = {}, reqMatch\n", session->user_id_);

	//매칭 처리
	if(session->is_matching_ == true || session->game_room_id_!=0 || session->room_id_==0 || session->is_logged_in_ == false)
	{
		return;
	}

	//room_id & session_id 필요. opponenet_id랑 game_room_id가 나오면 session 정보 수정.(game_room_id)
	auto [opponent_id, game_room_id] = game_room_manager_.Match(session->session_id_, session->room_id_);

	//매칭 결과 전송
	packet_sender_.ResMatch(session);

	//매칭이 되었다면
	if (opponent_id != 0)
	{
		//session에 game room id 저장
		session->game_room_id_ = game_room_id;
		auto oponnent_session = session_manager_.GetSession(opponent_id);
		oponnent_session->game_room_id_ = game_room_id;

		auto opponent_session = session_manager_.GetSession(opponent_id);
		packet_sender_.NtfMatched(session, opponent_session);
	}
}

void PacketProcessor::ReqReadyOmokHandler(Packet packet)
{
	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		return;
	}

	std::print("받은 메시지 : user_id = {}, reqReady\n", session->user_id_);

	//레디 처리
	if (session->game_room_id_==0 || session->is_ready_)
	{
		return;
	}

	session->is_ready_ = true;

	auto game_room = game_room_manager_.GetGameRoom(session->game_room_id_);
	game_room->SetReady(session->session_id_);

	//레디결과 전송
	packet_sender_.ResReadyOmok(session);

	if (game_room->IsGameStart())
	{
		auto black_id = game_room->GetBlackSessionId();
		auto black_session = session_manager_.GetSession(black_id);
		auto white_id = game_room->GetWhiteSessionId();
		auto white_session = session_manager_.GetSession(white_id);

		packet_sender_.NtfStartOmok(black_session, white_session);
	}
}

void PacketProcessor::ReqOmokPutHandler(Packet packet)
{
	OmokPacket::ReqPutMok req_put_mok;
	req_put_mok.ParseFromArray(packet.packet_body_, packet.packet_size_);

	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		return;
	}

	std::print("받은 메시지 : user_id = {}, ReqPutMok\n", session->user_id_);

	//돌 두기 처리
	uint32_t result = -1;
	auto game_room = game_room_manager_.GetGameRoom(session->game_room_id_);

	if (session->is_ready_ == true)
	{
		if (game_room->IsGameStart())
		{
			if (game_room->SetStone(req_put_mok.x(), req_put_mok.y(), session->session_id_))
			{
				result = 0;
			}
		}
	}

	//자신에게 돌두기 결과 전송
	packet_sender_.ResPutMok(session, result);

	//다른 유저에게 돌두기 전송
	if (result == 0)
	{
		auto opponent_id = game_room->GetOpponentId(session->session_id_);
		auto opponent_session = session_manager_.GetSession(opponent_id);

		packet_sender_.NtfPutMok(opponent_session, req_put_mok.x(), req_put_mok.y());

		//게임이 끝낫다면
		if (game_room->IsGameEnd())
		{
			auto winner_id = game_room->WinnerId();
			auto winner_session = session_manager_.GetSession(winner_id);
			auto loser_id = game_room->LoserId();
			auto loser_session = session_manager_.GetSession(loser_id);

			packet_sender_.NtfGameOver(winner_session, 1);
			packet_sender_.NtfGameOver(loser_session, 0);

			game_room_manager_.GameEnd(session->game_room_id_);

			session->game_room_id_ = 0;
			opponent_session->game_room_id_ = 0;
			session->is_ready_ = false;
			opponent_session->is_ready_ = false;
			session->is_matching_ = false;
			opponent_session->is_matching_ = false;
		}
	}
}