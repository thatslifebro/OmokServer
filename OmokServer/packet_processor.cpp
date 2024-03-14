#include "packet_processor.h"

void PacketProcessor::Init()
{
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
	
	//db_proccessor���� ����
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

	std::print("���� �޽��� : ReqRoomEnter - user_id = {}, room_number = {}\n", session->user_id_ ,req_room_enter.roomid());

	//�� ���� ó��
	uint32_t result = -1;

	if (session->is_logged_in_ == true && session->room_id_ == 0)
	{
		auto success = room_manager_.AddSession(session->session_id_, req_room_enter.roomid());
		if (success == true)
		{
			result = 0;
			session->room_id_ = req_room_enter.roomid();

			std::print("UserId : {} �� �� {}���� ������.\n", session->user_id_, session->room_id_);
		}
	}

	//�� ���� ��� ����
	packet_sender_.ResRoomEnter(session, result);

	// ������
	if (result == 0)
	{
		auto room_session_ids = room_manager_.GetSessionList(session->room_id_);
		//��������Ʈ ���� �� ���� ���� ����
		packet_sender_.NtfRoomUserList(session, room_session_ids);
		packet_sender_.NtfRoomUserEnter(room_session_ids, session);

		//������ �Ǿ��� ��
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

	std::print("���� �޽��� : ReqRoomLeave - user_id = {}\n", session->user_id_);

	auto room_id = session->room_id_;

	//�� ���� ó��
	uint32_t result = -1;

	if (session->is_logged_in_ == true && room_id != 0)
	{
		auto success = room_manager_.RemoveSession(session->session_id_, room_id);
		if (success == true)
		{
			result = 0;
			session->room_id_ = 0;

			std::print("UserId : {} �� �濡�� ����.\n", session->user_id_);
		}
	}

	//�� ���� ��� ����
	packet_sender_.ResRoomLeave(session, result);

	// ���� ��
	if (result == 0)
	{
		// ���� ���̾��ٸ�
		auto room = room_manager_.GetRoom(room_id);

		auto opponent_id = room->PlayerLeave(session->session_id_);
		if (opponent_id != 0)
		{
			auto opponent_session = session_manager_.GetSession(opponent_id);

			//���� ��� ����
			packet_sender_.NtfGameOver(opponent_session, 1);
			packet_sender_.NtfGameOver(session, 0);
		}

		if (room->IsMatched())
		{
			room->CancelMatch();
		}

		// ���� ����
		auto room_session_ids = room_manager_.GetSessionList(room_id);
		packet_sender_.NtfRoomUserLeave(room_session_ids, session);

		// ������ �����ٸ�
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

	std::print("���� �޽��� : ReqRoomChat - user_id = {}\n", session->user_id_);

	//��ȿ�� Ȯ��
	uint32_t result = -1;

	if (session->is_logged_in_ == true && session->room_id_ != 0)
	{
		result = 0;

		std::print("UserId : {} �� ä���� ����.\n", session->user_id_);
	}

	//�ڽſ��� ä�� ��� ����
	packet_sender_.ResRoomChat(session, result, req_room_chat.chat());

	//������ ����
	if (result == 0)
	{
		auto room_session_ids = room_manager_.GetSessionList(session->room_id_);
		packet_sender_.NtfRoomChat(room_session_ids, session, req_room_chat.chat());
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

	packet_sender_.NtfMatchReq(opponent_session, session);

	// Ÿ�̸� ����
	room->timer_->SetTimer(MATCH_RES_TIMEOUT, [room, session, opponent_id, opponent_session]()
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

	// Ÿ�̸ӿ� ���� �̹� ó���� ���
	auto already_done = room->timer_->IsCallbackDone();
	if (already_done)
	{
		return;
	}

	auto admin_session = session_manager_.GetSession(room->GetAdminId());
	packet_sender_.ResMatch(admin_session, result);

	if (result == 0)
	{
		packet_sender_.ResMatch(session, result);

		room->Matched();
		room->StartGame();

		room->timer_->SetTimer(READY_TIMEOUT, [room, session, admin_session]()
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
	packet_sender_.ResReadyOmok(session);

	//��� �غ�� ���� ���� �˸�
	if (game->IsGameStarted())
	{
		room->timer_->CancelTimer();

		auto black_id = game->GetBlackSessionId();
		auto black_session = session_manager_.GetSession(black_id);
		auto white_id = game->GetWhiteSessionId();
		auto white_session = session_manager_.GetSession(white_id);

		packet_sender_.NtfStartOmok(black_session, white_session);

		//������
		auto room_session_ids = room_manager_.GetSessionList(session->room_id_);
		packet_sender_.NtfStartOmokView(room_session_ids, black_session, white_session);

		//Ÿ�̸�
		// ���� ���� ���� �� ���� �ٲٴ� �ݹ� ���
		room->timer_->SetRepeatedTimer(PUT_MOK_TIMEOUT, [&, room, room_session_ids]()
			{
				PacketSender packet_sender;
				if (room->IsGameStarted())
				{
					auto game = room->GetGame();
					game->ChangeTurn();

					//Ÿ�Ӿƿ� ����
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

	std::print("���� �޽��� : ReqPutMok - user_id = {}\n", session->user_id_);

	//�� �α� ó��
	uint32_t result = -1;

	auto room = room_manager_.GetRoom(session->room_id_);
	if (room->IsGameStarted() == false)
	{
		return;
	}

	//Ÿ�̸Ӹ� ���� ���� �� �ٽ� �ѱ� 
	// �ϴ� �Ʒ� �ڵ�� ���ÿ� Ÿ�̸Ӱ� �ݹ��� ������ �� �ֱ� ������ Ÿ�̸Ӹ� ����.
	// Ÿ�̸ӿ� ���� ���� �ٲ���� ���� �ȹٲ���� ���� �ִ�.
	// � ��쿡�� �ڽ��� ���ʶ�� ���� �ΰ� �ƴ϶�� �ƹ��͵� ���� �ʴ´�.
	// ���� Ÿ�̸Ӹ� �ٽ� �Ҵ�.
	room->timer_->CancelTimer();

	auto game = room->GetGame();

	// ���α� ������
	if (game->SetStone(req_put_mok.x(), req_put_mok.y(), session->session_id_))
	{
		result = 0;
	}

	//�ڽſ��� ��� ����
	packet_sender_.ResPutMok(session, result);

	if (result == -1)
	{
		room->timer_->ContinueTimer();
	}
	//�ٸ� �������� ��� ����
	else
	{
		//�ٸ� ��ο��� ����
		auto room_session_ids = room_manager_.GetSessionList(session->room_id_);
		packet_sender_.NtfPutMok(room_session_ids, session, req_put_mok.x(), req_put_mok.y());

		//�̹� ���α�� ������ �����ٸ�
		if (game->IsGameEnd())
		{
			auto winner_id = game->WinnerId();
			auto winner_session = session_manager_.GetSession(winner_id);
			auto loser_id = game->LoserId();
			auto loser_session = session_manager_.GetSession(loser_id);

			//���� ��� ����
			packet_sender_.NtfGameOver(winner_session, 1);
			packet_sender_.NtfGameOver(loser_session, 0);

			packet_sender_.NtfGameOverView(room_session_ids, winner_id, loser_id, 2);

			//���� ���� ó��
			room->EndGame();
		}
		else
		{
			room->timer_->num_ = 0;
			//Ÿ�̸� �ٽ� ����
			// ���� ���� ���� �� ���� �ٲٴ� �ݹ� ���
			room->timer_->SetRepeatedTimer(PUT_MOK_TIMEOUT, [&, room, room_session_ids]()
				{
					PacketSender packet_sender;
					if (room->IsGameStarted())
					{
						auto game = room->GetGame();
						game->ChangeTurn();

						//Ÿ�Ӿƿ� ����
						packet_sender.NtfPutMokTimeout(room_session_ids);
					}
				});
		}
	}
}