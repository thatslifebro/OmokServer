#include "packet_processor.h"

void PacketProcessor::Init()
{
	packet_sender_.Init([&](uint32_t session_id, std::shared_ptr<char[]> buffer, int length) {
		auto session = session_manager_.GetSession(session_id);
		session->SendPacket(buffer, length);
		});

	//handler ���
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
	PacketQueue packet_queue_;
	auto packet = packet_queue_.PopAndGetPacket();
	try
	{
		if (packet.packet_size_ <= 0)
		{
			return false;
		}

		packet_handler_map_[packet.packet_id_](packet);
		return true;
	}
	catch (ErrorCode error_code)
	{
		std::print("ErrorCode : {}, PacketId = {}\n", static_cast<int>(error_code), packet.packet_id_);
		return false; 
	}
	
}

void PacketProcessor::ReqLoginHandler(Packet packet)
{
	OmokPacket::ReqLogin req_login;
	req_login.ParseFromArray(packet.packet_body_, packet.packet_size_ - PacketHeader::header_size_);
	
	//db_proccessor���� ����
	DBPacketQueue db_packet_queue_;
	db_packet_queue_.PushPacket(packet);
}

void PacketProcessor::ReqRoomEnterHandler(Packet packet)
{
	OmokPacket::ReqRoomEnter req_room_enter;
	req_room_enter.ParseFromArray(packet.packet_body_, packet.packet_size_ - PacketHeader::header_size_);
	auto room_id = req_room_enter.roomid();

	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		throw ErrorCode::InvalidSessionId;
	}

	auto room = room_manager_.GetRoom(room_id);
	if (room == nullptr)
	{
		packet_sender_.ResRoomEnter(session->session_id_, session->user_id_, static_cast<int>(ErrorCode::InvalidRoomId));
		throw ErrorCode::InvalidRoomId;
	}
	if (session->is_logged_in_ == false)
	{
		packet_sender_.ResRoomEnter(session->session_id_, session->user_id_, static_cast<int>(ErrorCode::NotLoggedIn));
		throw ErrorCode::NotLoggedIn;
	}
	if (session->room_id_ != 0)
	{
		packet_sender_.ResRoomEnter(session->session_id_, session->user_id_, static_cast<int>(ErrorCode::AlreadyInRoom));
		throw ErrorCode::AlreadyInRoom;
	}
	if (room->IsGameStarted())
	{
		packet_sender_.ResRoomEnter(session->session_id_, session->user_id_, static_cast<int>(ErrorCode::RoomGameStarted));
		throw ErrorCode::RoomGameStarted;
	}

	//�� ���� ó��
	room_manager_.AddUser(session->session_id_, room_id);
	session->room_id_ = room_id;
	std::print("UserId : {} �� �� {}���� ������.\n", session->user_id_, session->room_id_);

	//�� ���� ���, ���� ����Ʈ, ���� ���� ����
	packet_sender_.ResRoomEnter(session->session_id_, session->user_id_, 0);
	room->ResRoomUserList(session->session_id_);
	if(room->IsAdmin(session->session_id_))
	{
		packet_sender_.ResYouAreRoomAdmin(session->session_id_);
	}
	else
	{
		room->ResRoomAdmin(session->session_id_);
	}
	// �� ���� ����
	room->NtfRoomUserEnter(session->session_id_);
}

void PacketProcessor::ReqRoomLeaveHandler(Packet packet)
{
	OmokPacket::ReqRoomLeave req_room_leave;
	req_room_leave.ParseFromArray(packet.packet_body_, packet.packet_size_ - PacketHeader::header_size_);

	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		throw ErrorCode::InvalidSessionId;
	}
	if (session->is_logged_in_ == false)
	{
		packet_sender_.ResRoomLeave(session->session_id_, static_cast<int>(ErrorCode::NotLoggedIn));
		throw ErrorCode::NotLoggedIn;
	}
	if (session->room_id_ == 0)
	{
		packet_sender_.ResRoomLeave(session->session_id_, static_cast<int>(ErrorCode::NotInRoom));
		throw ErrorCode::NotInRoom;
	}

	// �� ������ ó��
	room_manager_.RemoveUser(session->session_id_, session->room_id_);
	session->room_id_ = 0;

	std::print("UserId : {} �� �濡�� ����.\n", session->user_id_);

	auto room = room_manager_.GetRoom(session->room_id_);
	//�� ���� ��� ���� �� ����
	packet_sender_.ResRoomLeave(session->session_id_, 0);
	room->NtfRoomUserLeave(session->session_id_);
	
	// ���� ���̾��ٸ� ���� ��� ����
	if (room->IsPlayer(session->session_id_))
	{
		auto opponent_id = room->GetOpponentPlayer(session->session_id_);
		packet_sender_.NtfGameOver(opponent_id, 1);
		packet_sender_.NtfGameOver(session->session_id_, 0);
	}

	// ��Ī �� ���¿��ٸ� ��Ī �ʱ�ȭ
	if (room->IsMatched())
	{
		room->EndMatch();
	}	

	// ���� ���� ó��
	if (room->IsAdmin(session->session_id_))
	{
		room->ChangeAdmin();
		packet_sender_.ResYouAreRoomAdmin(room->GetAdminId());
		room->NtfNewRoomAdmin();
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

	std::print("���� �޽��� : ReqRoomChat - user_id = {}\n", session->user_id_);

	//��ȿ�� Ȯ��
	uint32_t result = -1;

	if (session->is_logged_in_ == true && session->room_id_ != 0)
	{
		result = 0;

		std::print("UserId : {} �� ä���� ����.\n", session->user_id_);
	}

	//�ڽſ��� ä�� ��� ����
	packet_sender_.ResRoomChat(session->session_id_, result, req_room_chat.chat());

	//������ ����
	if (result == 0)
	{
		auto room = room_manager_.GetRoom(session->room_id_);
		room->NtfRoomChat(session->session_id_, req_room_chat.chat());
	}
}

void PacketProcessor::ReqMatchHandler(Packet packet)
{
	// ������ ��Ī ��û
	OmokPacket::ReqMatch req_match;
	req_match.ParseFromArray(packet.packet_body_, packet.packet_size_ - PacketHeader::header_size_);

	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		return;
	}

	std::print("���� �޽��� : ReqMatch - user_id = {}, reqMatch\n", session->user_id_);

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

	// ��Ī ó��

	room->TryMatchingWith(opponent_id);

	packet_sender_.NtfMatchReq(opponent_id, session->session_id_, session->user_id_);

	// Ÿ�̸� ����
	room->SetTimer(time_count_, MATCH_RES_TIMEOUT, [&, room, session, opponent_id]()
		{
			if (room->IsTryMatchingWith(opponent_id))
			{
				room->CancelMatch();
				packet_sender_.ResMatch(session->session_id_, -1);
				packet_sender_.NtfMatchTimeout(opponent_id);
			}
		});
}

void PacketProcessor::ReqMatchRes(Packet packet)
{
	//������ ��û�� ���� ����
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

	// ��Ī ó��

	uint32_t result = -1;

	if(req_match_res.accept() == false)
	{
		room->CancelMatch();
	}
	else
	{
		result = 0;
	}

	packet_sender_.ResMatch(room->GetAdminId(), result);

	room->CancelTimer();

	if (result == 0)
	{
		packet_sender_.ResMatch(session->session_id_, result);

		room->MatchComplete(room->GetAdminId(), session->session_id_);

		room->SetTimer(time_count_, READY_TIMEOUT, [&, room, session]()
			{
				if (room->IsGameStarted() == false)
				{
					room->EndMatch();
					packet_sender_.NtfReadyTimeout(session->session_id_);
					packet_sender_.NtfReadyTimeout(room->GetAdminId());
				}
			});
	}
}

void PacketProcessor::ReqReadyOmokHandler(Packet packet)
{
	// body ���� ��Ŷ

	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		return;
	}

	std::print("���� �޽��� : ReqReady - user_id = {}\n", session->user_id_);

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

	//�غ� ��� ����
	packet_sender_.ResReadyOmok(session->session_id_);

	//��� �غ�� ���� ���� �˸�
	if (game->IsGameStarted())
	{
		auto black_id = game->GetBlackSessionId();
		auto black_session = session_manager_.GetSession(black_id);
		auto white_id = game->GetWhiteSessionId();
		auto white_session = session_manager_.GetSession(white_id);

		packet_sender_.NtfStartOmok(black_id, black_session->user_id_, white_id, white_session->user_id_);

		//������
		room->NtfStartOmokView(black_id, black_session->user_id_, white_id, white_session->user_id_);

		//Ÿ�̸�
		// ���� ���� ���� �� ���� �ٲٴ� �ݹ� ���
		room->SetRepeatedTimer(time_count_, PUT_MOK_TIMEOUT, [&, room]()
			{
				if (room->IsGameStarted())
				{
					auto game = room->GetGame();
					game->ChangeTurn();

					//Ÿ�Ӿƿ� ����
					packet_sender_.NtfPutMokTimeout(room->GetSessionIds());
				}
			});
	}
}

void PacketProcessor::ReqOmokPutHandler(Packet packet)
{
	OmokPacket::ReqPutMok req_put_mok;
	req_put_mok.ParseFromArray(packet.packet_body_, packet.packet_size_ - PacketHeader::header_size_);
	auto x = req_put_mok.x();
	auto y = req_put_mok.y();

	auto session = session_manager_.GetSession(packet.session_id_);
	if (session == nullptr)
	{
		return;
	}

	std::print("���� �޽��� : ReqPutMok - user_id = {}\n", session->user_id_);

	//�� �α� ó��
	uint32_t result = -1;

	auto room = room_manager_.GetRoom(session->room_id_);
	if (room->IsGameStarted() == false)
	{
		return;
	}

	auto game = room->GetGame();

	// ���α� ������
	auto success = game->SetStone(x, y, session->session_id_);
	if (success)
	{
		result = 0;
		room->CancelTimer();
	}

	//�ڽſ��� ��� ����
	packet_sender_.ResPutMok(session->session_id_, result);

	//�ٸ� �������� ��� ����
	if(result == 0)
	{
		//�ٸ� ��ο��� ����
		room->NtfPutMok(session->session_id_, x, y);

		//�̹� ���α�� ������ �����ٸ�
		if (game->CheckOmok(x, y))
		{
			auto winner_id = game->WinnerId();
			auto loser_id = game->LoserId();

			//���� ��� ����
			packet_sender_.NtfGameOver(winner_id, 1);
			packet_sender_.NtfGameOver(loser_id, 0);

			room->NtfGameOverView(winner_id, loser_id, 2);

			//���� ���� ó��
			room->EndMatch();
		}
		else
		{
			//Ÿ�̸� �ٽ� ����
			room->SetSameWithPreviousTimer(time_count_);
		}
	}
}

void PacketProcessor::TimerCheck()
{
	time_count_++;
	
	//1�ʸ��� Ÿ�̸� 10�� üũ (��� �ϳ��̱� ����)
	auto room_list = room_manager_.GetAllRooms();
	for (auto room : room_list)
	{
		room->TimerCheck(time_count_);
	}
}