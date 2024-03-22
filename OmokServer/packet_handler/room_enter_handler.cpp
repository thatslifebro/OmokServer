#include "room_enter_handler.h"

void RoomEnterHandler::Init(std::function<void(uint32_t session_id, uint32_t room_id)> UserEnterRoom,
	std::function<Room* (uint32_t room_id)> GetRoom,
	std::function<Session* (uint32_t session_id)> GetSession)
{
	packet_sender_.InitSendPacketFunc([&](uint32_t session_id, char* buffer, int length) {
		auto session = GetSession_(session_id);
		session->SendPacket(buffer, length); });

	UserEnterRoom_ = UserEnterRoom;
	GetRoom_ = GetRoom;
	GetSession_ = GetSession;
}

ErrorCode RoomEnterHandler::HandleRequest(Packet packet)
{
	auto room_id = ExtractPacketData(packet);

	auto session = GetSession_(packet.GetSessionId());
	if (session == nullptr)
	{
		return ErrorCode::InvalidSession;
	}

	auto session_id = session->GetSessionId();
	auto user_id = session->GetUserId();
	auto room = GetRoom_(room_id);

	auto error_code = CheckError(session_id, session, user_id, room);
	if (error_code != ErrorCode::None)
	{
		return error_code;
	}

	std::print("UserId : {} 가 방 {}번에 입장함.\n", user_id, room_id);

	ProcessRequest(session_id, session, user_id, room_id, room);

	return ErrorCode::None;
}

uint32_t RoomEnterHandler::ExtractPacketData(Packet packet)
{
	OmokPacket::ReqRoomEnter req_room_enter;
	req_room_enter.ParseFromArray(packet.GetPacketBody(), packet.GetBodySize());

	auto room_id = req_room_enter.roomid();

	return room_id;
}

void RoomEnterHandler::ProcessRequest(uint32_t session_id, Session* session, std::string user_id, uint32_t room_id, Room* room)
{
	UserEnterRoom_(session_id, room_id);
	session->SetRoomId(room_id);

	packet_sender_.ResRoomEnter(session_id, user_id, 0);
	room->ResRoomUserList(session_id);

	if (room->IsAdmin(session_id))
	{
		packet_sender_.ResYouAreRoomAdmin(session_id);
	}
	else
	{
		auto admin_id = room->GetAdminId();
		packet_sender_.ResRoomAdmin(session_id, admin_id, GetSession_(admin_id)->GetUserId());
	}

	room->NtfRoomUserEnter(session_id);
}

ErrorCode RoomEnterHandler::CheckError(uint32_t session_id, Session* session, std::string user_id, Room * room)
{
	if (room == nullptr)
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