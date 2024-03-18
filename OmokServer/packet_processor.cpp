#include "packet_processor.h"

void PacketProcessor::Init()
{
	packet_sender_.Init([&](uint32_t session_id, std::shared_ptr<char[]> buffer, int length) {
		auto session = session_manager_.GetSession(session_id);
		session->SendPacket(buffer, length);
		});

	//handler 등록
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqLogin), [&](Packet packet) -> ErrorCode { return ReqLoginHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqRoomEnter), [&](Packet packet) -> ErrorCode { return ReqRoomEnterHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqRoomLeave), [&](Packet packet) -> ErrorCode { return ReqRoomLeaveHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqRoomChat), [&](Packet packet) -> ErrorCode { return ReqRoomChatHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqMatch), [&](Packet packet) -> ErrorCode { return ReqMatchHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqReadyOmok), [&](Packet packet) -> ErrorCode { return ReqReadyOmokHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqPutMok), [&](Packet packet) -> ErrorCode { return ReqOmokPutHandler(packet); }));
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqMatchRes), [&](Packet packet) -> ErrorCode { return ReqMatchResHandler(packet); }));
}

bool PacketProcessor::ProcessPacket() // todo : bool 리턴 해야하나
{
	auto packet = PopAndGetPacket();
	if (packet.IsValidSize() == false)
	{
		return false;
	}

	auto result = packet_handler_map_[packet.GetPacketId()](packet);
	if (result != ErrorCode::None)
	{
		std::print("PacketId : {} 처리 중 에러 발생. ErrorCode : {}\n", packet.GetPacketId(), static_cast<int>(result));
	}

	return true;	
}

ErrorCode PacketProcessor::ReqLoginHandler(Packet packet)
{
	DBPacketQueue db_packet_queue_; // todo : function으로 받아서 하면 좋을 듯.
	db_packet_queue_.PushPacket(packet);

	return ErrorCode::None;
}

ErrorCode PacketProcessor::ReqRoomEnterHandler(Packet packet)
{
	OmokPacket::ReqRoomEnter req_room_enter;
	req_room_enter.ParseFromArray(packet.GetPacketBody(), packet.GetBodySize());
	auto room_id = req_room_enter.roomid();

	auto session = session_manager_.GetSession(packet.GetSessionId());
	if (IsValidSession(session) == false)
	{
		return ErrorCode::InvalidSessionId;
	}	

	auto session_id = session->GetSessionId();
	auto user_id = session->GetUserId();
	auto room = room_manager_.GetRoom(room_id);

	if (IsValidRoom(room) == false)
	{
		packet_sender_.ResRoomEnter(session_id, user_id, static_cast<int>(ErrorCode::InvalidRoomId));
		return ErrorCode::InvalidRoomId;
	}

	if (session->IsLoggedIn() == false)
	{
		packet_sender_.ResRoomEnter(session_id, user_id, static_cast<int>(ErrorCode::NotLoggedIn));
		return ErrorCode::NotLoggedIn;
	}

	if (session->IsInRoom())
	{
		packet_sender_.ResRoomEnter(session_id, user_id, static_cast<int>(ErrorCode::AlreadyInRoom));
		return ErrorCode::AlreadyInRoom;
	}

	if (room->IsGameStarted())
	{
		packet_sender_.ResRoomEnter(session_id, user_id, static_cast<int>(ErrorCode::RoomGameStarted));
		return ErrorCode::RoomGameStarted;
	}

	room_manager_.AddUser(session_id, room_id);
	session->SetRoomId(room_id);

	std::print("UserId : {} 가 방 {}번에 입장함.\n", user_id, room_id);

	packet_sender_.ResRoomEnter(session_id, user_id, 0);
	room->ResRoomUserList(session_id);

	if(room->IsAdmin(session_id))
	{
		packet_sender_.ResYouAreRoomAdmin(session_id);
	}
	else
	{
		room->ResRoomAdmin(session_id);
	}

	room->NtfRoomUserEnter(session_id);

	return ErrorCode::None;
}

ErrorCode PacketProcessor::ReqRoomLeaveHandler(Packet packet)
{
	auto session = session_manager_.GetSession(packet.GetSessionId());
	if (IsValidSession(session) == false)
	{
		return ErrorCode::InvalidSessionId;
	}

	auto session_id = session->GetSessionId();
	auto room_id = session->GetRoomId();
	auto room = room_manager_.GetRoom(room_id);

	if (session->IsLoggedIn() == false)
	{
		packet_sender_.ResRoomLeave(session_id, static_cast<int>(ErrorCode::NotLoggedIn));
		return ErrorCode::NotLoggedIn;
	}

	if (session->IsInRoom() == false)
	{
		packet_sender_.ResRoomLeave(session_id, static_cast<int>(ErrorCode::NotInRoom));
		return ErrorCode::NotInRoom;
	}

	room_manager_.RemoveUser(session_id, room_id);
	session->SetRoomId(0);

	std::print("UserId : {} 가 방에서 나감.\n", session->GetUserId());

	packet_sender_.ResRoomLeave(session_id, 0);
	room->NtfRoomUserLeave(session_id);
	
	if (room->IsGameStarted() && room->IsPlayer(session_id))
	{
		auto opponent_id = room->GetOpponentPlayer(session_id);
		packet_sender_.NtfGameOver(opponent_id, 1);
		packet_sender_.NtfGameOver(session_id, 0);
		std::print("{}번 방 게임 종료. {} 승리 ,{} 패배\n", room_id, session_manager_.GetSession(opponent_id)->GetUserId(), session->GetUserId());
	}

	if (room->IsMatched())
	{
		room->EndMatch();
	}	

	if (room->IsAdmin(session_id))
	{
		room->ChangeAdmin();

		auto new_admin_id = room->GetAdminId();
		packet_sender_.ResYouAreRoomAdmin(new_admin_id);

		room->NtfNewRoomAdmin();
	}

	return ErrorCode::None;
}

ErrorCode PacketProcessor::ReqRoomChatHandler(Packet packet)
{
	OmokPacket::ReqRoomChat req_room_chat;
	req_room_chat.ParseFromArray(packet.GetPacketBody(), packet.GetBodySize());
	auto chat = req_room_chat.chat();

	auto session = session_manager_.GetSession(packet.GetSessionId());
	if (session == nullptr)
	{
		return ErrorCode::InvalidSessionId;
	}

	auto session_id = session->GetSessionId();
	auto room = room_manager_.GetRoom(session->GetRoomId());

	if (session->IsLoggedIn() == false)
	{
		packet_sender_.ResRoomChat(session_id, static_cast<int>(ErrorCode::NotLoggedIn), "");
		return ErrorCode::NotLoggedIn;
	}

	if (session->IsInRoom() == false)
	{
		packet_sender_.ResRoomChat(session_id, static_cast<int>(ErrorCode::NotInRoom), "");
		return ErrorCode::NotInRoom;
	}

	std::print("UserId : {} 가 채팅을 보냄.\n", session->GetUserId());

	packet_sender_.ResRoomChat(session_id, 0, chat);	
	room->NtfRoomChat(session_id, chat);

	return ErrorCode::None;
}

ErrorCode PacketProcessor::ReqMatchHandler(Packet packet)
{
	// 방장의 매칭 요청
	OmokPacket::ReqMatch req_match;
	req_match.ParseFromArray(packet.GetPacketBody(), packet.GetBodySize());

	auto session = session_manager_.GetSession(packet.GetSessionId());
	if (IsValidSession(session) == false)
	{
		return ErrorCode::InvalidSessionId;
	}

	auto session_id = session->GetSessionId();
	auto opponent_id = req_match.sessionid();
	auto opponent_session = session_manager_.GetSession(opponent_id);
	auto room = room_manager_.GetRoom(session->GetRoomId());

	if (IsValidSession(opponent_session) == false)
	{
		return ErrorCode::InvalidSessionId;
	}

	if (session->IsLoggedIn() == false)
	{
		return ErrorCode::NotLoggedIn;
	}

	if (session->IsInRoom() == false)
	{
		return ErrorCode::NotInRoom;
	}
	
	if (room->IsTryMatching())
	{
		return ErrorCode::AlreadyTryMatching;
	}

	if (room->IsAdmin(session_id) == false)
	{
		return ErrorCode::NotRoomAdmin;
	}

	if (session_id == opponent_id)
	{
		return ErrorCode::SameSessionIdWithOpponent;
	}

	std::print("방장 {} 이 매칭 요청을 {} 에게 보냄.\n", session->GetUserId(), opponent_session->GetUserId());

	room->TryMatchingWith(opponent_id);

	packet_sender_.NtfMatchReq(opponent_id, session_id, session->GetUserId());

	room->SetTimer(time_count_, MATCH_RES_TIMEOUT, [&]()
		{
			if (room->IsTryMatchingWith(opponent_id))
			{
				room->CancelMatch();
				packet_sender_.ResMatch(session_id, static_cast<uint32_t>(ErrorCode::TimeOut));
				packet_sender_.NtfMatchTimeout(opponent_id);
				std::print("매칭 요청 시간 초과 : {} -> {}\n", session->GetUserId(), opponent_session->GetUserId());
			}
		});

	return ErrorCode::None;
}

ErrorCode PacketProcessor::ReqMatchResHandler(Packet packet)
{
	//방장의 요청에 대한 응답
	OmokPacket::ReqMatchRes req_match_res;
	req_match_res.ParseFromArray(packet.GetPacketBody(), packet.GetBodySize());
	auto accept = req_match_res.accept();

	auto session = session_manager_.GetSession(packet.GetSessionId());
	if (IsValidSession(session) == false)
	{
		return ErrorCode::InvalidSessionId;
	}

	auto session_id = session->GetSessionId();
	auto room = room_manager_.GetRoom(session->GetRoomId());
	auto admin_id = room->GetAdminId();

	if (room->IsTryMatchingWith(session_id) == false)
	{
		packet_sender_.ResMatch(admin_id, static_cast<uint32_t>(ErrorCode::AlreadyTryMatching));
		return ErrorCode::AlreadyTryMatching;
	}

	if(accept == false)
	{
		room->CancelMatch();

		packet_sender_.ResMatch(admin_id, 1);

		room->CancelTimer();

		std::print("매칭 거절 : {} -> {}\n", session->GetUserId(), room->GetUserId(admin_id));
	}
	else
	{
		packet_sender_.ResMatch(admin_id, 0);

		packet_sender_.ResMatch(session_id, 0);

		room->MatchComplete(admin_id, session_id);

		room->SetTimer(time_count_, READY_TIMEOUT, [&]()
			{
				if (room->IsGameStarted() == false)
				{
					room->EndMatch();
					packet_sender_.NtfReadyTimeout(session_id);
					packet_sender_.NtfReadyTimeout(admin_id);
					std::print("{}번 방 준비 완료 시간 초과\n", session->GetRoomId());
				}
			});

		std::print("매칭 수락 : {} -> {}\n", session->GetUserId(), room->GetUserId(admin_id));
	}
	
	return ErrorCode::None;
}

ErrorCode PacketProcessor::ReqReadyOmokHandler(Packet packet)
{
	auto session = session_manager_.GetSession(packet.GetSessionId());
	if (IsValidSession(session) == false)
	{
		return ErrorCode::InvalidSessionId;
	}

	auto session_id = session->GetSessionId();
	auto room = room_manager_.GetRoom(session->GetRoomId());
	auto game = room->GetGame();

	if (session->IsInRoom() == false)
	{
		return ErrorCode::NotInRoom;
	}

	if (room->IsPlayer(session_id) == false)
	{
		return ErrorCode::NotPlayer;
	}

	if (room->IsGameStarted() == true)
	{
		return ErrorCode::GameAlreadyStarted;
	}

	if (game->IsReady(session_id))
	{
		return ErrorCode::AlreadyReady;
	}

	game->SetReady(session_id);

	packet_sender_.ResReadyOmok(session_id);
	
	std::print("UserId : {} 준비 완료.\n", session->GetUserId());

	game->StartGameIfBothReady();

	if (room->IsGameStarted())
	{
		std::print("{}번 Room 게임 시작!\n", session->GetRoomId());

		auto [black_session_id, black_user_id, white_session_id, white_user_id] = room->GetPlayerInfo();

		packet_sender_.NtfStartOmok(black_session_id, black_user_id, white_session_id, white_user_id);

		room->NtfStartOmokView(black_session_id, black_user_id, white_session_id, white_user_id);

		room->SetRepeatedTimer(time_count_, PUT_MOK_TIMEOUT, [&]()
			{
				if (room->IsGameStarted())
				{
					auto game = room->GetGame();
					game->ChangeTurn();

					room->NtfPutMokTimeout();
					std::print("PutMok 시간 초과\n");
				}
			});
	}

	return ErrorCode::None;
}

ErrorCode PacketProcessor::ReqOmokPutHandler(Packet packet)
{
	OmokPacket::ReqPutMok req_put_mok;
	req_put_mok.ParseFromArray(packet.GetPacketBody(), packet.GetBodySize());
	auto x = req_put_mok.x();
	auto y = req_put_mok.y();

	auto session = session_manager_.GetSession(packet.GetSessionId());
	if (IsValidSession(session) == false)
	{
		return ErrorCode::InvalidSessionId;
	}

	auto session_id = session->GetSessionId();
	auto room = room_manager_.GetRoom(session->GetRoomId());
	auto game = room->GetGame();
	
	if (room->IsGameStarted() == false)
	{
		packet_sender_.ResPutMok(session_id, static_cast<uint32_t>(ErrorCode::GameNotStarted));
		return ErrorCode::GameNotStarted;
	}
	if (room->IsPlayer(session_id) == false)
	{
		packet_sender_.ResPutMok(session_id, static_cast<uint32_t>(ErrorCode::NotPlayer));
		return ErrorCode::NotPlayer;
	}
	if (game->CheckTurn(session_id) == false)
	{
		packet_sender_.ResPutMok(session_id, static_cast<uint32_t>(ErrorCode::NotYourTurn));
		return ErrorCode::NotYourTurn;
	}

	auto success = game->SetStone(x, y, session_id);
	if (success == false)
	{
		packet_sender_.ResPutMok(session_id, static_cast<uint32_t>(ErrorCode::PutMokFail));
	}
	else
	{
		packet_sender_.ResPutMok(session_id, 0);
		room->CancelTimer();

		room->NtfPutMok(session_id, x, y);
		
		std::print("UserId : {} 가 ({}, {})에 돌을 놓음.\n", session->GetUserId(), x, y);

		if (game->CheckOmok(x, y) == true)
		{
			auto winner_id = game->WinnerId();
			auto loser_id = game->LoserId();

			packet_sender_.NtfGameOver(winner_id, 1);
			packet_sender_.NtfGameOver(loser_id, 0);

			room->NtfGameOverView(winner_id, loser_id);

			//게임 종료 처리
			room->EndMatch();

			std::print("{}번 방 게임 종료. {} 승리 ,{} 패배\n", session->GetRoomId(), session_manager_.GetSession(winner_id)->GetUserId(), session_manager_.GetSession(loser_id)->GetUserId());
		}
		else
		{
			//타이머 다시 설정
			room->SetSameWithPreviousTimer(time_count_);
		}
	}

	return ErrorCode::None;
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
bool PacketProcessor::IsValidSession(Session* session)
{
	if (session == nullptr)
	{
		return false;
	}
	return true;
}

bool PacketProcessor::IsValidRoom(Room* room)
{
	if (room == nullptr)
	{
		return false;
	}
	return true;
}