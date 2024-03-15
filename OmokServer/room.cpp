#include "room.h"

void Room::Init()
{
	packet_sender_.SendPacket = SendPacket;

	NetworkFunctionInit();

	game_ = new Game();
}

void Room::AddUser(uint16_t session_id)
{
	if (session_ids_.empty())
	{
		admin_id_ = session_id;
	}
	session_ids_.insert(session_id);
}

void Room::RemoveUser(uint16_t session_id)
{
	session_ids_.erase(session_id);
	if (admin_id_ == session_id)
	{
		ChangeAdmin();
	}
}

std::unordered_set<uint32_t> Room::GetSessionIds() const
{
	return session_ids_;
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

void Room::Matched(uint32_t white_session_id, uint32_t black_session_id)
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

uint32_t Room::PlayerLeave(uint32_t session_id)
{
	if (IsGameStarted() == false)
	{
		return 0;
	}

	if (opponent_id_ == session_id || admin_id_ == session_id)
	{
		timer_->CancelTimer();
		auto opponent_id = game_->GetOpponentId(session_id);
		EndGame();
		return opponent_id;
	}

	return 0;
}

void Room::EndGame() {
	game_->EndGame();
	try_matching_ = false;
	matched_ = false;
	opponent_id_ = 0;
}

void Room::NetworkFunctionInit()
{
	NtfRoomUserList = [&](uint32_t session_id) {
		std::vector<std::pair<uint32_t, std::string>> user_list;
		for (auto other_session_id : session_ids_)
		{
			if (other_session_id == session_id)
			{
				continue;
			}

			user_list.push_back(std::make_pair(session_id, GetUserId(session_id)));
		}
		packet_sender_.NtfRoomUserList(session_id, user_list);
		};

	NtfRoomUserEnter = [&](uint32_t session_id) {
		packet_sender_.NtfRoomUserEnter(session_ids_, session_id, GetUserId(session_id)); };

	NtfRoomUserLeave = [&](uint32_t session_id) {
		packet_sender_.NtfRoomUserLeave(session_ids_, session_id, GetUserId(session_id));
		};

	NtfNewRoomAdmin = [&](uint32_t session_id) {
		packet_sender_.NtfNewRoomAdmin(session_ids_, session_id, GetUserId(session_id)); };

	NtfRoomChat = [&](uint32_t session_id, std::string chat) {
		packet_sender_.NtfRoomChat(session_ids_, session_id, GetUserId(session_id), chat); };

	NtfStartOmokView = [&](uint32_t black_session_id, std::string balck_user_id, uint32_t white_session_id, std::string white_user_id) { packet_sender_.NtfStartOmokView(session_ids_, black_session_id, balck_user_id, white_session_id, white_user_id); };

	NtfPutMok = [&](uint32_t session_id, uint32_t x, uint32_t y) {
		packet_sender_.NtfPutMok(session_ids_, session_id, x, y); };

	NtfGameOverView = [&](uint32_t winner_id, uint32_t loser_id, uint32_t result) { packet_sender_.NtfGameOverView(session_ids_, winner_id, loser_id, result); };
}