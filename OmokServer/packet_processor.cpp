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
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqMatchRes), [&](Packet packet) { ReqMatchRes(packet); }));
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
	
	//db_proccessor에게 전달
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
		auto room = room_manager_.GetRoom(req_room_enter.roomid());
		if (room == nullptr)
		{
			return;
		}
		if (room->IsGameStarted())
		{
			return;
		}
		else {
			auto success = room_manager_.AddSession(session->session_id_, req_room_enter.roomid());
			if (success == true)
			{
				result = 0;
				session->room_id_ = req_room_enter.roomid();

				std::print("UserId : {} 가 방 {}번에 입장함.\n", session->user_id_, session->room_id_);
			}
		}
	}

	//방 입장 결과 전송
	packet_sender_.ResRoomEnter(session, result);

	// 성공시
	if (result == 0)
	{
		auto room_session_ids = room_manager_.GetSessionList(session->room_id_);
		//유저리스트 전달 및 유저 입장 전파
		packet_sender_.NtfRoomUserList(session, room_session_ids);
		packet_sender_.NtfRoomUserEnter(room_session_ids, session);

		//방장이 되었을 때
		auto room = room_manager_.GetRoom(session->room_id_);
		if(room->IsAdmin(session->session_id_))
		{
			packet_sender_.NtfRoomAdmin(session);
		}
		else
		{
			auto admin_session = session_manager_.GetSession(room->GetAdminId());
			packet_sender_.NtfNewRoomAdmin(session, admin_session);
		}
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

	// 성공 시
	if (result == 0)
	{
		// 게임 중이었다면
		auto room = room_manager_.GetRoom(room_id);

		auto opponent_id = room->PlayerLeave(session->session_id_);
		if (opponent_id != 0)
		{
			auto opponent_session = session_manager_.GetSession(opponent_id);

			//게임 결과 전송
			packet_sender_.NtfGameOver(opponent_session, 1);
			packet_sender_.NtfGameOver(session, 0);
		}

		if (room->IsMatched())
		{
			room->CancelMatch();
		}

		// 나감 전파
		auto room_session_ids = room_manager_.GetSessionList(room_id);
		packet_sender_.NtfRoomUserLeave(room_session_ids, session);

		// 방장이 나갔다면
		if (room->IsAdmin(session->session_id_))
		{
			room->ChangeAdmin();

			auto admin_session = session_manager_.GetSession(room->GetAdminId());
			if (admin_session == nullptr)
			{
				return;
			}

			packet_sender_.NtfRoomAdmin(admin_session);
			packet_sender_.NtfNewRoomAdmin(room_session_ids, admin_session);
		}
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

	//유효성 확인
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
	// 방장의 매칭 요청
	OmokPacket::ReqMatch req_match;
	req_match.ParseFromArray(packet.packet_body_, packet.packet_size_ - PacketHeader::header_size_);

	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		return;
	}

	std::print("받은 메시지 : ReqMatch - user_id = {}, reqMatch\n", session->user_id_);

	auto opponent_id = req_match.sessionid();
	auto opponent_session = session_manager_.GetSession(opponent_id);
	if (opponent_session == nullptr)
	{
		return;
	}

	if (session->room_id_ == 0 || session->is_logged_in_ == false)
	{
		return;
	}

	auto room = room_manager_.GetRoom(session->room_id_);
	if (room->IsTryMatching()|| room->IsAdmin(session->session_id_) == false || session->session_id_ == req_match.sessionid())
	{
		return;
	}

	// 매칭 처리

	room->TryMatchingWith(opponent_id);

	packet_sender_.NtfMatchReq(opponent_session, session);

	// 타이머 시작
	room->SetTimer(time_count_, MATCH_RES_TIMEOUT, [room, session, opponent_id, opponent_session]()
		{
			PacketSender packet_sender;
			if (room->IsTryMatchingWith(opponent_id))
			{
				room->CancelMatch();
				packet_sender.ResMatch(session, -1);
				packet_sender.NtfMatchTimeout(opponent_session);
			}
		});
}

void PacketProcessor::ReqMatchRes(Packet packet)
{
	//방장의 요청에 대한 응답
	OmokPacket::ReqMatchRes req_match_res;
	req_match_res.ParseFromArray(packet.packet_body_, packet.packet_size_ - PacketHeader::header_size_);

	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		return;
	}

	auto room = room_manager_.GetRoom(session->room_id_);
	if (room->IsTryMatchingWith(session->session_id_) == false)
	{
		return;
	}

	// 매칭 처리

	uint32_t result = -1;

	if(req_match_res.accept() == false)
	{
		room->CancelMatch();
	}
	else
	{
		result = 0;
	}

	auto admin_session = session_manager_.GetSession(room->GetAdminId());
	packet_sender_.ResMatch(admin_session, result);

	room->CancelTimer();

	if (result == 0)
	{
		packet_sender_.ResMatch(session, result);

		room->Matched();
		room->StartGame();

		room->SetTimer(time_count_, READY_TIMEOUT, [room, session, admin_session]()
			{
				PacketSender packet_sender;
				if (room->IsGameStarted() == false)
				{
					room->EndGame();
					packet_sender.NtfReadyTimeout(session);
					packet_sender.NtfReadyTimeout(admin_session);
				}
			});
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

	if (session->room_id_ == 0)
	{
		return;
	}

	auto room = room_manager_.GetRoom(session->room_id_);

	if (room->IsAdmin(session->session_id_) == false && room->IsOpponent(session->session_id_) == false)
	{
		return;
	}

	auto game = room->GetGame();

	if (game == nullptr)
	{
		return;
	}

	if (game->IsReady(session->session_id_) || game->IsGameStarted())
	{
		return;
	}

	game->SetReady(session->session_id_);

	//준비 결과 전송
	packet_sender_.ResReadyOmok(session);

	//모두 준비시 게임 시작 알림
	if (game->IsGameStarted())
	{
		auto black_id = game->GetBlackSessionId();
		auto black_session = session_manager_.GetSession(black_id);
		auto white_id = game->GetWhiteSessionId();
		auto white_session = session_manager_.GetSession(white_id);

		packet_sender_.NtfStartOmok(black_session, white_session);

		//관전자
		auto room_session_ids = room_manager_.GetSessionList(session->room_id_);
		packet_sender_.NtfStartOmokView(room_session_ids, black_session, white_session);

		//타이머
		// 돌을 두지 않을 시 턴을 바꾸는 콜백 등록
		room->SetRepeatedTimer(time_count_, PUT_MOK_TIMEOUT, [&, room, room_session_ids]()
			{
				PacketSender packet_sender;
				if (room->IsGameStarted())
				{
					auto game = room->GetGame();
					game->ChangeTurn();

					//타임아웃 전파
					packet_sender.NtfPutMokTimeout(room_session_ids);
				}
			});
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

	auto room = room_manager_.GetRoom(session->room_id_);
	if (room->IsGameStarted() == false)
	{
		return;
	}

	auto game = room->GetGame();

	// 돌두기 성공시
	if (game->SetStone(req_put_mok.x(), req_put_mok.y(), session->session_id_))
	{
		result = 0;
		room->CancelTimer();
	}

	//자신에게 결과 전송
	packet_sender_.ResPutMok(session, result);

	//다른 유저에게 결과 전송
	if(result == 0)
	{
		//다른 모두에게 전달
		auto room_session_ids = room_manager_.GetSessionList(session->room_id_);
		packet_sender_.NtfPutMok(room_session_ids, session, req_put_mok.x(), req_put_mok.y());

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

			packet_sender_.NtfGameOverView(room_session_ids, winner_id, loser_id, 2);

			//게임 종료 처리
			room->EndGame();
		}
		else
		{
			//타이머 다시 설정
			room->SetSameWithPreviousTimer(time_count_);
		}
	}
}

void PacketProcessor::TimerCheck()
{
	time_count_++;
	
	//1초마다 타이머 10개 체크 (방당 하나이기 때문)
	auto room_list = room_manager_.GetAllRooms();
	for (auto room : room_list)
	{
		room->TimerCheck(time_count_);
	}
}