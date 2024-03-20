#include "packet_processor.h"

void PacketProcessor::Init()
{
	packet_sender_.InitSendPacketFunc([&](uint32_t session_id, char* buffer, int length) {
		auto session = GetSession_(session_id);
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
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqRemoveSession), [&](Packet packet) -> ErrorCode { return ReqRemoveSession(packet); }));
}

void PacketProcessor::InitPacketQueueFunctions(std::function<const Packet& ()> PopAndGetPacket, std::function<void(const Packet&)> PushDBPacket)
{
	PopAndGetPacket_ = PopAndGetPacket;
	PushDBPacket_ = PushDBPacket;
}

void PacketProcessor::InitRoomManagerFunctions(std::function<void(uint32_t session_id, uint16_t room_id)> AddUser, std::function<void(uint32_t session_id, uint16_t room_id)> RemoveUser, std::function<Room* (uint16_t room_id)> GetRoom, std::function<std::vector<Room*>()> GetAllRooms)
{
	AddUser_ = AddUser;
	RemoveUser_ = RemoveUser;
	GetRoom_ = GetRoom;
	GetAllRooms_ = GetAllRooms;
}

bool PacketProcessor::ProcessPacket()
{
	auto packet = PopAndGetPacket_();
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
	PushDBPacket_(packet);

	return ErrorCode::None;
}

ErrorCode PacketProcessor::ReqRoomEnterHandler(Packet packet)
{
	auto room_id = ReqRoomEnterPacketData(packet);

	auto session = GetSession_(packet.GetSessionId());
	if (IsValidSession(session) == false)
	{
		return ErrorCode::InvalidSession;
	}

	auto session_id = session->GetSessionId();
	auto user_id = session->GetUserId();
	auto room = GetRoom_(room_id);

	auto error_code = ReqRoomEnterErrorCheck(session, session_id, user_id, room);
	if (error_code != ErrorCode::None)
	{
		return error_code;
	}

	std::print("UserId : {} 가 방 {}번에 입장함.\n", user_id, room_id);

	ReqRoomEnterProcess(session_id, session, user_id, room_id, room);

	return ErrorCode::None;
}

uint32_t PacketProcessor::ReqRoomEnterPacketData(Packet packet)
{
	OmokPacket::ReqRoomEnter req_room_enter;
	req_room_enter.ParseFromArray(packet.GetPacketBody(), packet.GetBodySize());

	auto room_id = req_room_enter.roomid();

	return room_id;
}

void PacketProcessor::ReqRoomEnterProcess(uint32_t session_id, Session* session, std::string user_id, uint32_t room_id, Room* room)
{
	AddUser_(session_id, room_id);
	session->SetRoomId(room_id);

	packet_sender_.ResRoomEnter(session_id, user_id, 0);
	room->ResRoomUserList_(session_id);

	if (room->IsAdmin(session_id))
	{
		packet_sender_.ResYouAreRoomAdmin(session_id);
	}
	else
	{
		auto admin_id = room->GetAdminId();
		packet_sender_.ResRoomAdmin(session_id, admin_id, GetSession_(admin_id)->GetUserId());
	}

	room->NtfRoomUserEnter_(session_id);
}

ErrorCode PacketProcessor::ReqRoomEnterErrorCheck(Session* session, uint32_t session_id, std::string user_id, Room* room)
{
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

	return ErrorCode::None;
}

ErrorCode PacketProcessor::ReqRoomLeaveHandler(Packet packet)
{
	auto session = GetSession_(packet.GetSessionId());
	if (IsValidSession(session) == false)
	{
		return ErrorCode::InvalidSession;
	}

	auto session_id = session->GetSessionId();
	auto room_id = session->GetRoomId();
	auto room = GetRoom_(room_id);

	auto error_code = ReqRoomLeaveErrorCheck(session, session_id);
	if (error_code != ErrorCode::None)
	{
		return error_code;
	}

	std::print("UserId : {} 가 방에서 나감.\n", session->GetUserId());

	ReqRoomLeaveProcess(session, session_id, room, room_id);

	return ErrorCode::None;
}

void PacketProcessor::ReqRoomLeaveProcess(Session* session, uint32_t session_id, Room* room, uint32_t room_id)
{
	RemoveUser_(session_id, room_id);
	session->SetRoomId(0);

	packet_sender_.ResRoomLeave(session_id, 0);
	room->NtfRoomUserLeave_(session_id);

	if (room->IsGameStarted() && room->IsPlayer(session_id))
	{
		auto opponent_id = room->GetOpponentPlayer(session_id);
		packet_sender_.NtfGameOver(opponent_id, 1);
		packet_sender_.NtfGameOver(session_id, 0);
		std::print("{}번 방 게임 종료. {} 승리 ,{} 패배\n", room_id, GetSession_(opponent_id)->GetUserId(), session->GetUserId());
	}

	if (room->IsMatched())
	{
		room->EndMatch();
	}

	if (room->IsAdmin(session_id))
	{
		room->ChangeAdmin();

		if (room->IsEmpty() == false)
		{
			auto new_admin_id = room->GetAdminId();
			packet_sender_.ResYouAreRoomAdmin(new_admin_id);

			room->NtfNewRoomAdmin_();
		}
	}
}

ErrorCode PacketProcessor::ReqRoomLeaveErrorCheck(Session* session, uint32_t session_id)
{
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

	return ErrorCode::None;
}


ErrorCode PacketProcessor::ReqRoomChatHandler(Packet packet)
{
	auto chat = ReqRoomChatPacketData(packet);

	auto session = GetSession_(packet.GetSessionId());
	if (session == nullptr)
	{
		return ErrorCode::InvalidSession;
	}

	auto session_id = session->GetSessionId();
	auto room = GetRoom_(session->GetRoomId());

	auto error_code = ReqRoomChatErrorCheck(session, session_id);
	if (error_code != ErrorCode::None)
	{
		return error_code;
	}

	std::print("UserId : {} 가 채팅을 보냄.\n", session->GetUserId());

	ReqRoomChatProcess(session_id, chat, room);

	return ErrorCode::None;
}

std::string PacketProcessor::ReqRoomChatPacketData(Packet packet)
{
	OmokPacket::ReqRoomChat req_room_chat;
	req_room_chat.ParseFromArray(packet.GetPacketBody(), packet.GetBodySize());
	
	auto chat = req_room_chat.chat();

	return chat;
}

void PacketProcessor::ReqRoomChatProcess(uint32_t session_id, std::string chat, Room* room)
{
	packet_sender_.ResRoomChat(session_id, 0, chat);

	room->NtfRoomChat_(session_id, chat);
}

ErrorCode PacketProcessor::ReqRoomChatErrorCheck(Session* session, uint32_t session_id)
{
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

	return ErrorCode::None;
}

ErrorCode PacketProcessor::ReqMatchHandler(Packet packet)
{
	// 방장의 매칭 요청
	auto opponent_id = ReqMatchPacketData(packet);

	auto session = GetSession_(packet.GetSessionId());
	if (IsValidSession(session) == false)
	{
		return ErrorCode::InvalidSession;
	}

	auto session_id = session->GetSessionId();
	auto opponent_session = GetSession_(opponent_id);
	auto room = GetRoom_(session->GetRoomId());

	auto error_code = ReqMatchErrorCheck(session_id, session, opponent_id, opponent_session, room);
	if (error_code != ErrorCode::None)
	{
		return error_code;
	}

	std::print("방장 {} 이 매칭 요청을 {} 에게 보냄.\n", session->GetUserId(), opponent_session->GetUserId());

	ReqMatchProcess(session_id, session, opponent_id, opponent_session, room);

	return ErrorCode::None;
}

uint32_t PacketProcessor::ReqMatchPacketData(Packet packet)
{
	OmokPacket::ReqMatch req_match;
	req_match.ParseFromArray(packet.GetPacketBody(), packet.GetBodySize());

	auto opponent_id = req_match.sessionid();

	return opponent_id;
}

void PacketProcessor::ReqMatchProcess(uint32_t session_id, Session* session, uint32_t opponent_id, Session* opponent_session, Room* room)
{
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
}

ErrorCode PacketProcessor::ReqMatchErrorCheck(uint32_t session_id, Session* session, uint32_t opponent_id, Session* opponent_session, Room* room)
{
	if (IsValidSession(opponent_session) == false)
	{
		return ErrorCode::InvalidSession;
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

	return ErrorCode::None;
}

ErrorCode PacketProcessor::ReqMatchResHandler(Packet packet)
{
	//방장의 요청에 대한 응답
	auto accept = ReqMatchResPacketData(packet);

	auto session = GetSession_(packet.GetSessionId());
	if (IsValidSession(session) == false)
	{
		return ErrorCode::InvalidSession;
	}

	auto session_id = session->GetSessionId();
	auto room = GetRoom_(session->GetRoomId());
	auto admin_id = room->GetAdminId();

	auto error_code = ReqMatchResErrorCheck(session_id, admin_id, room);
	if (error_code != ErrorCode::None)
	{
		return error_code;
	}

	ReqMatchResProcess(session_id, session, admin_id, accept, room);
	
	return ErrorCode::None;
}

bool PacketProcessor::ReqMatchResPacketData(Packet packet)
{
	OmokPacket::ReqMatchRes req_match_res;
	req_match_res.ParseFromArray(packet.GetPacketBody(), packet.GetBodySize());

	auto accept = req_match_res.accept();

	return accept;
}

void PacketProcessor::ReqMatchResProcess(uint32_t session_id, Session* session, uint32_t admin_id, bool accept, Room* room)
{
	if (accept == false)
	{
		room->CancelMatch();

		packet_sender_.ResMatch(admin_id, 1);

		room->CancelTimer();

		std::print("매칭 거절 : {} -> {}\n", session->GetUserId(), GetSession_(admin_id)->GetUserId());
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

		std::print("매칭 수락 : {} -> {}\n", session->GetUserId(), GetSession_(admin_id)->GetUserId());
	}
}

ErrorCode PacketProcessor::ReqMatchResErrorCheck(uint32_t session_id, uint32_t admin_id, Room* room)
{
	if (room->IsTryMatchingWith(session_id) == false)
	{
		packet_sender_.ResMatch(admin_id, static_cast<uint32_t>(ErrorCode::AlreadyTryMatching));
		return ErrorCode::AlreadyTryMatching;
	}

	return ErrorCode::None;
}

ErrorCode PacketProcessor::ReqReadyOmokHandler(Packet packet)
{
	auto session = GetSession_(packet.GetSessionId());
	if (IsValidSession(session) == false)
	{
		return ErrorCode::InvalidSession;
	}

	auto session_id = session->GetSessionId();
	auto room = GetRoom_(session->GetRoomId());
	auto game = room->GetGame();

	auto error_code = ReqReadyOmokErrorCheck(session_id, session, room, game);
	if (error_code != ErrorCode::None)
	{
		return error_code;
	}

	std::print("UserId : {} 준비 완료.\n", session->GetUserId());

	ReqReadyOmokProcess(session_id, session, room, game);

	return ErrorCode::None;
}

void PacketProcessor::ReqReadyOmokProcess(uint32_t session_id, Session* session, Room* room, Game* game)
{
	game->SetReady(session_id);

	packet_sender_.ResReadyOmok(session_id);

	game->StartGameIfBothReady();

	if (room->IsGameStarted())
	{
		std::print("{}번 Room 게임 시작!\n", session->GetRoomId());

		auto [black_session_id, black_user_id, white_session_id, white_user_id] = room->GetPlayerInfo();

		packet_sender_.NtfStartOmok(black_session_id, black_user_id, white_session_id, white_user_id);

		room->NtfStartOmokView_(black_session_id, black_user_id, white_session_id, white_user_id);

		room->SetRepeatedTimer(time_count_, PUT_MOK_TIMEOUT, [room]()
			{
				if (room->IsGameStarted())
				{
					auto game = room->GetGame();
					game->ChangeTurn();

					room->NtfPutMokTimeout_();
					std::print("PutMok 시간 초과\n");
				}
			});
	}
}

ErrorCode PacketProcessor::ReqReadyOmokErrorCheck(uint32_t session_id, Session* session, Room* room, Game* game)
{
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

	return ErrorCode::None;
}

ErrorCode PacketProcessor::ReqOmokPutHandler(Packet packet)
{
	auto [x, y] = ReqOmokPutPacketData(packet);

	auto session = GetSession_(packet.GetSessionId());
	if (IsValidSession(session) == false)
	{
		return ErrorCode::InvalidSession;
	}

	auto session_id = session->GetSessionId();
	auto room = GetRoom_(session->GetRoomId());
	auto game = room->GetGame();
	
	auto error_code = ReqOmokPutErrorCheck(session_id, room, game, x, y);
	if (error_code != ErrorCode::None)
	{
		return error_code;
	}

	std::print("UserId : {} 가 ({}, {})에 돌을 놓음.\n", session->GetUserId(), x, y);

	ReqOmokPutProcess(session_id, room, game, x, y);

	return ErrorCode::None;
}

std::tuple<uint32_t, uint32_t> PacketProcessor::ReqOmokPutPacketData(Packet packet)
{
	OmokPacket::ReqPutMok req_put_mok;
	req_put_mok.ParseFromArray(packet.GetPacketBody(), packet.GetBodySize());

	auto x = req_put_mok.x();
	auto y = req_put_mok.y();

	return std::make_tuple(x, y);
}

void PacketProcessor::ReqOmokPutProcess(uint32_t session_id, Room* room, Game* game, uint32_t x, uint32_t y)
{
	game->SetStone(x, y, session_id);

	packet_sender_.ResPutMok(session_id, 0);
	room->CancelTimer();

	room->NtfPutMok_(session_id, x, y);

	if (game->CheckOmok(x, y) == true)
	{
		auto winner_id = game->WinnerId();
		auto loser_id = game->LoserId();

		packet_sender_.NtfGameOver(winner_id, 1);
		packet_sender_.NtfGameOver(loser_id, 0);

		room->NtfGameOverView_(winner_id, loser_id);

		//게임 종료 처리
		room->EndMatch();

		std::print("{}번 방 게임 종료. {} 승리 ,{} 패배\n", GetSession_(session_id)->GetRoomId(), GetSession_(winner_id)->GetUserId(), GetSession_(loser_id)->GetUserId());
	}
	else
	{
		//타이머 다시 설정
		room->SetSameWithPreviousTimer(time_count_);
	}
}

ErrorCode PacketProcessor::ReqOmokPutErrorCheck(uint32_t session_id, Room* room, Game* game, uint32_t x, uint32_t y)
{
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

	if (game->CanSetStone(x, y, session_id) == false)
	{
		packet_sender_.ResPutMok(session_id, static_cast<uint32_t>(ErrorCode::PutMokFail));
		return ErrorCode::PutMokFail;
	}

	return ErrorCode::None;
}

ErrorCode PacketProcessor::ReqRemoveSession(Packet packet)
{
	auto session_id = packet.GetSessionId();
	RemoveSession_(session_id);

	return ErrorCode::None;
}

void PacketProcessor::TimerCheck()
{
	time_count_++;
	
	//1초마다 타이머 10개 체크 (방당 하나이기 때문)
	auto room_list = GetAllRooms_();
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