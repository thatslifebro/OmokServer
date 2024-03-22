#include "room_chat_handler.h"

void RoomChatHandler::Init(std::function<Room* (uint32_t room_id)> GetRoom,
	std::function<Session* (uint32_t session_id)> GetSession)
{
	packet_sender_.InitSendPacketFunc([&](uint32_t session_id, char* buffer, int length) {
		auto session = GetSession_(session_id);
		session->SendPacket(buffer, length); });

	GetRoom_ = GetRoom;
	GetSession_ = GetSession;
}

ErrorCode RoomChatHandler::HandleRequest(Packet packet)
{
	auto chat = ExtractPacketData(packet);

	auto session = GetSession_(packet.GetSessionId());
	if (session == nullptr)
	{
		return ErrorCode::InvalidSession;
	}

	auto session_id = session->GetSessionId();
	auto room = GetRoom_(session->GetRoomId());

	auto error_code = CheckError(session_id, session);
	if (error_code != ErrorCode::None)
	{
		return error_code;
	}

	std::print("UserId : {} 가 채팅을 보냄.\n", session->GetUserId());

	ProcessRequest(session_id, chat, room);

	return ErrorCode::None;
}

std::string RoomChatHandler::ExtractPacketData(Packet packet)
{
	OmokPacket::ReqRoomChat req_room_chat;
	req_room_chat.ParseFromArray(packet.GetPacketBody(), packet.GetBodySize());

	auto chat = req_room_chat.chat();

	return chat;
}

void RoomChatHandler::ProcessRequest(uint32_t session_id, std::string chat, Room* room)
{
	packet_sender_.ResRoomChat(session_id, 0, chat);

	room->NtfRoomChat(session_id, chat);
}

ErrorCode RoomChatHandler::CheckError(uint32_t session_id, Session* session)
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