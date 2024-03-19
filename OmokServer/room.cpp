#include "room.h"

void Room::Init(std::function<void(uint32_t, std::shared_ptr<char[]>, uint16_t)> SendPacket,
	std::function<std::string(uint32_t)> GetUserId)
{
	timer_ = new Timer();
	game_ = new Game();

	packet_sender_.InitSendPacketFunc(SendPacket);

	GetUserId_ = GetUserId;

	InitNetworkFunctions();	
}

void Room::AddUser(uint16_t session_id)
{
	if (session_ids_.empty())
	{
		admin_id_ = session_id;
	}

	session_ids_.insert(session_id);
}

void Room::ChangeAdmin()
{
	if (!session_ids_.empty())
	{
		admin_id_ = *session_ids_.begin();
	}
	else
	{
		admin_id_ = 0;
	}
}

void Room::TryMatchingWith(uint32_t opponent_id)
{
	opponent_id_ = opponent_id;
	try_matching_ = true;
}

void Room::MatchComplete(uint32_t white_session_id, uint32_t black_session_id)
{
	matched_ = true;

	game_->Init(white_session_id, black_session_id);
}

void Room::CancelMatch()
{
	try_matching_ = false;
	matched_ = false;
	opponent_id_ = 0;
}

std::tuple<uint32_t, std::string, uint32_t, std::string> Room::GetPlayerInfo() const
{
	auto black_user_id = game_->GetBlackSessionId();
	auto white_user_id = game_->GetWhiteSessionId();

	return std::make_tuple(black_user_id, GetUserId_(black_user_id), white_user_id, GetUserId_(white_user_id));
}

uint32_t Room::GetOpponentPlayer(uint32_t session_id)
{
	if (session_id == admin_id_)
	{
		return opponent_id_;
	}
	else
	{
		return admin_id_;
	}
}

bool Room::IsPlayer(uint32_t session_id)
{
	if (opponent_id_ == session_id || admin_id_ == session_id)
	{
		return true;
	}

	return false;
}

void Room::EndMatch()
{
	timer_->CancelTimer();

	game_->EndGame();

	CancelMatch();
}

void Room::InitNetworkFunctions()
{
	ResRoomUserList_ = [&](uint32_t session_id)
		{
			std::vector<std::pair<uint32_t, std::string>> user_list;
			for (auto other_session_id : session_ids_)
			{
				if (other_session_id == session_id)
				{
					continue;
				}

				user_list.push_back(std::make_pair(other_session_id, GetUserId_(other_session_id)));
			}
			packet_sender_.ResRoomUserList(session_id, user_list);
		};

	NtfRoomUserEnter_ = [&](uint32_t session_id) { packet_sender_.NtfRoomUserEnter(session_ids_, session_id, GetUserId_(session_id)); };

	NtfRoomUserLeave_ = [&](uint32_t session_id) { packet_sender_.NtfRoomUserLeave(session_ids_, session_id, GetUserId_(session_id)); };

	NtfNewRoomAdmin_ = [&] { packet_sender_.NtfNewRoomAdmin(session_ids_, admin_id_, GetUserId_(admin_id_)); };

	NtfRoomChat_ = [&](uint32_t session_id, std::string chat) { packet_sender_.NtfRoomChat(session_ids_, session_id, GetUserId_(session_id), chat); };

	NtfStartOmokView_ = [&](uint32_t black_session_id, std::string balck_user_id, uint32_t white_session_id, std::string white_user_id) 
		{ packet_sender_.NtfStartOmokView(session_ids_, black_session_id, balck_user_id, white_session_id, white_user_id); };

	NtfPutMok_ = [&](uint32_t session_id, uint32_t x, uint32_t y) { packet_sender_.NtfPutMok(session_ids_, session_id, x, y); };

	NtfGameOverView_ = [&](uint32_t winner_id, uint32_t loser_id) { packet_sender_.NtfGameOverView(session_ids_, winner_id, loser_id); };

	NtfPutMokTimeout_ = [&]{ packet_sender_.NtfPutMokTimeout(session_ids_); };
}