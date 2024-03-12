#include "packet_processor.h"

void PacketProcessor::Init()
{
	//handler 등록
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqLogin), [&](Packet packet) { ReqLoginHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqRoomEnter), [&](Packet packet) { ReqRoomEnterHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqRoomLeave), [&](Packet packet) { ReqRoomLeaveHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqRoomChat), [&](Packet packet) { ReqRoomChatHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqMatch), [&](Packet packet) { ReqMatchHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqReadyOmok), [&](Packet packet) { ReqReadyOmokHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqPutMok), [&](Packet packet) { ReqOmokPutHandler(packet); }));
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
	req_login.ParseFromArray(packet.packet_body_, packet.packet_size_ - PacketHeader::header_size_);
	
	db_packet_queue_.PushPacket(packet);
}

void PacketProcessor::ReqRoomEnterHandler(Packet packet)
{
	OmokPacket::ReqRoomEnter req_room_enter;
	req_room_enter.ParseFromArray(packet.packet_body_, packet.packet_size_ - PacketHeader::header_size_);
	
	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		return;
	}

	std::print("받은 메시지 : ReqRoomEnter - user_id = {}, room_number = {}\n", session->user_id_ ,req_room_enter.roomid());

	//방 입장 처리
	uint32_t result = -1;

	if (session->is_logged_in_ == true && session->room_id_ == 0)
	{
		auto success = room_manager_.AddSession(session->session_id_, req_room_enter.roomid());
		if (success == true)
		{
			result = 0;
			session->room_id_ = req_room_enter.roomid();

			std::print("UserId : {} 가 방 {}번에 입장함.\n", session->user_id_, session->room_id_);
		}
	}

	//방 입장 결과 전송
	packet_sender_.ResRoomEnter(session, result);
	
	// 성공시 유저리스트 전달 및 전파
	if (result == 0)
	{
		auto room_session_ids = room_manager_.GetSessionList(session->room_id_);
		packet_sender_.NtfRoomUserList(session, room_session_ids);
		packet_sender_.NtfRoomUserEnter(room_session_ids, session);
	}
}

void PacketProcessor::ReqRoomLeaveHandler(Packet packet)
{
	OmokPacket::ReqRoomLeave req_room_leave;
	req_room_leave.ParseFromArray(packet.packet_body_, packet.packet_size_ - PacketHeader::header_size_);

	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		return;
	}

	std::print("받은 메시지 : ReqRoomLeave - user_id = {}\n", session->user_id_);

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

			std::print("UserId : {} 가 방에서 나감.\n", session->user_id_);
		}
	}

	//방 나감 결과 전송
	packet_sender_.ResRoomLeave(session, result);

	//성공시 전파
	if (result == 0)
	{
		auto room_session_ids = room_manager_.GetSessionList(room_id);
		packet_sender_.NtfRoomUserLeave(room_session_ids, session);
	}
}

void PacketProcessor::ReqRoomChatHandler(Packet packet)
{
	OmokPacket::ReqRoomChat req_room_chat;
	req_room_chat.ParseFromArray(packet.packet_body_, packet.packet_size_ - PacketHeader::header_size_);

	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		return;
	}

	std::print("받은 메시지 : ReqRoomChat - user_id = {}\n", session->user_id_);

	//채팅 처리
	uint32_t result = -1;

	if (session->is_logged_in_ == true && session->room_id_ != 0)
	{
		result = 0;

		std::print("UserId : {} 가 채팅을 보냄.\n", session->user_id_);
	}

	//자신에게 채팅 결과 전송
	packet_sender_.ResRoomChat(session, result, req_room_chat.chat());

	//성공시 전파
	if (result == 0)
	{
		auto room_session_ids = room_manager_.GetSessionList(session->room_id_);
		packet_sender_.NtfRoomChat(room_session_ids, session, req_room_chat.chat());
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

	std::print("받은 메시지 : ReqMatch - user_id = {}, reqMatch\n", session->user_id_);

	//매칭 처리
	if (session->is_matching_ == true || session->game_id_!=0 || session->room_id_==0 || session->is_logged_in_ == false)
	{
		return;
	}

	auto [opponent_id, game_id] = match_manager_.Match(session->session_id_, session->room_id_);

	//매칭 응답 전송
	packet_sender_.ResMatch(session);

	//상대와 매칭 성공시 매칭 상대 정보 전송
	if (opponent_id != 0)
	{
		session->game_id_ = game_id;
		auto opponent_session = session_manager_.GetSession(opponent_id);
		opponent_session->game_id_ = game_id;

		packet_sender_.NtfMatched(session, opponent_session);
	}
}

void PacketProcessor::ReqReadyOmokHandler(Packet packet)
{
	// body 없는 패킷

	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		return;
	}

	std::print("받은 메시지 : ReqReady - user_id = {}\n", session->user_id_);

	//준비 처리
	if (session->game_id_==0 || session->is_ready_)
	{
		return;
	}

	session->is_ready_ = true;

	auto game = match_manager_.GetGame(session->game_id_);
	game->SetReady(session->session_id_);

	//준비 결과 전송
	packet_sender_.ResReadyOmok(session);

	//모두 준비시 게임 시작 알림
	if (game->IsGameStart())
	{
		auto black_id = game->GetBlackSessionId();
		auto black_session = session_manager_.GetSession(black_id);
		auto white_id = game->GetWhiteSessionId();
		auto white_session = session_manager_.GetSession(white_id);

		packet_sender_.NtfStartOmok(black_session, white_session);
	}
}

void PacketProcessor::ReqOmokPutHandler(Packet packet)
{
	OmokPacket::ReqPutMok req_put_mok;
	req_put_mok.ParseFromArray(packet.packet_body_, packet.packet_size_ - PacketHeader::header_size_);

	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		return;
	}

	std::print("받은 메시지 : ReqPutMok - user_id = {}\n", session->user_id_);

	//돌 두기 처리
	uint32_t result = -1;
	auto game = match_manager_.GetGame(session->game_id_);

	if (session->is_ready_ == true && game->IsGameStart())
	{
		// 돌두기 성공시
		if (game->SetStone(req_put_mok.x(), req_put_mok.y(), session->session_id_))
		{
			result = 0;
		}
	}

	//자신에게 결과 전송
	packet_sender_.ResPutMok(session, result);

	//다른 유저에게 결과 전송
	if (result == 0)
	{
		auto opponent_id = game->GetOpponentId(session->session_id_);
		auto opponent_session = session_manager_.GetSession(opponent_id);

		packet_sender_.NtfPutMok(opponent_session, req_put_mok.x(), req_put_mok.y());

		//이번 돌두기로 게임이 끝났다면
		if (game->IsGameEnd())
		{
			auto winner_id = game->WinnerId();
			auto winner_session = session_manager_.GetSession(winner_id);
			auto loser_id = game->LoserId();
			auto loser_session = session_manager_.GetSession(loser_id);

			//게임 결과 전송
			packet_sender_.NtfGameOver(winner_session, 1);
			packet_sender_.NtfGameOver(loser_session, 0);

			//게임 종료 처리
			match_manager_.GameEnd(session->game_id_);

			//초기화
			session->game_id_ = 0;
			opponent_session->game_id_ = 0;
			session->is_ready_ = false;
			opponent_session->is_ready_ = false;
			session->is_matching_ = false;
			opponent_session->is_matching_ = false;
		}
	}
}