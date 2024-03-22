#include "packet_processor.h"

void PacketProcessor::Init()
{
	packet_sender_.InitSendPacketFunc([&](uint32_t session_id, char* buffer, int length) {
		auto session = GetSession_(session_id);
		session->SendPacket(buffer, length); });

	InitPacketHandler();

	InitSaveHandleRequestFunc();
}

void PacketProcessor::InitPacketHandler()
{
	room_enter_handler_.Init(UserEnterRoom_, GetRoom_, GetSession_);
	room_leave_handler_.Init(UserLeaveRoom_, GetRoom_, GetSession_);
	room_chat_handler_.Init(GetRoom_, GetSession_);
	match_req_handler_.Init(GetRoom_, GetSession_);
	match_res_handler_.Init(GetRoom_, GetSession_);
	ready_handler_.Init(GetRoom_, GetSession_);
	put_mok_handler_.Init(GetRoom_, GetSession_);
}

void PacketProcessor::InitSaveHandleRequestFunc()
{
	packet_handler_map_[static_cast<uint16_t>(PacketId::ReqLogin)] = [&](Packet packet) { return ReqLoginHandler(packet); };
	packet_handler_map_[static_cast<uint16_t>(PacketId::ReqRoomEnter)] = [&](Packet packet) { return room_enter_handler_.HandleRequest(packet); };
	packet_handler_map_[static_cast<uint16_t>(PacketId::ReqRoomLeave)] = [&](Packet packet) { return room_leave_handler_.HandleRequest(packet); };
	packet_handler_map_[static_cast<uint16_t>(PacketId::ReqRoomChat)] = [&](Packet packet) { return room_chat_handler_.HandleRequest(packet); };
	packet_handler_map_[static_cast<uint16_t>(PacketId::ReqMatch)] = [&](Packet packet) { return match_req_handler_.HandleRequest(packet, time_count_); };
	packet_handler_map_[static_cast<uint16_t>(PacketId::ReqMatchRes)] = [&](Packet packet) { return match_res_handler_.HandleRequest(packet, time_count_); };
	packet_handler_map_[static_cast<uint16_t>(PacketId::ReqReadyOmok)] = [&](Packet packet) { return ready_handler_.HandleRequest(packet, time_count_); };
	packet_handler_map_[static_cast<uint16_t>(PacketId::ReqPutMok)] = [&](Packet packet) { return put_mok_handler_.HandleRequest(packet, time_count_); };

	packet_handler_map_[static_cast<uint16_t>(PacketId::ReqAddSession)] = [&](Packet packet) { return ReqAddSession(packet); };
	packet_handler_map_[static_cast<uint16_t>(PacketId::ReqRemoveSession)] = [&](Packet packet) { return ReqRemoveSession(packet); };
}

void PacketProcessor::InitPacketQueueFunctions(std::function<Packet()> PopAndGetPacket, std::function<void(Packet)> PushDBPacket)
{
	PopAndGetPacket_ = PopAndGetPacket;
	PushDBPacket_ = PushDBPacket;
}

void PacketProcessor::InitRoomManagerFunctions(std::function<void(uint32_t session_id, uint32_t room_id)> UserEnterRoom, std::function<void(uint32_t session_id, uint32_t room_id)> UserLeaveRoom, std::function<Room* (uint32_t room_id)> GetRoom, std::function<std::vector<Room*>()> GetAllRooms)
{
	UserEnterRoom_ = UserEnterRoom;
	UserLeaveRoom_ = UserLeaveRoom;
	GetRoom_ = GetRoom;
	GetAllRooms_ = GetAllRooms;
}

void PacketProcessor::InitSessionManagerFunctions(std::function<uint32_t(Session*)> AddSession,
	std::function<Session* (uint32_t session_id)> GetSession,
	std::function<void(uint32_t)> RemoveSession)
{
	AddSession_ = AddSession;
	GetSession_ = GetSession;
	RemoveSession_ = RemoveSession;
}

bool PacketProcessor::ProcessPacket()
{
	auto packet = PopAndGetPacket_();
	if (packet.IsValidSize() == false)
	{
		return false;
	}

	if (packet_handler_map_.find(packet.GetPacketId()) == packet_handler_map_.end())
	{
		std::print("PacketId : {} 처리 함수 없음.\n", packet.GetPacketId());
		return false;
	}

	auto error_code = packet_handler_map_[packet.GetPacketId()](packet);
	if (error_code != ErrorCode::None)
	{
		std::print("PacketId : {} 처리 중 에러 발생. ErrorCode : {}\n", packet.GetPacketId(), static_cast<int>(error_code));
	}

	return true;	
}

ErrorCode PacketProcessor::ReqLoginHandler(Packet packet)
{
	PushDBPacket_(packet);

	return ErrorCode::None;
}

ErrorCode PacketProcessor::ReqAddSession(Packet packet)
{
	uint64_t session_addr;
	memcpy(&session_addr, packet.GetPacketBody(), sizeof(session_addr));

	auto session = reinterpret_cast<Session*>(session_addr);

	auto session_id = AddSession_(session);
	session->SetSessionId(session_id);

	return ErrorCode::None;
}

ErrorCode PacketProcessor::ReqRemoveSession(Packet packet)
{
	auto session_id = packet.GetSessionId();
	auto session = GetSession_(session_id);

	if (session->IsInRoom())
	{
		auto room_id = session->GetRoomId();
		auto room = GetRoom_(room_id);
		
		std::print("UserId : {} 가 방에서 나감.\n", session->GetUserId());

		ReqRemoveSessionRoomLeaveProcess(session, session_id, room, room_id);
	}

	RemoveSession_(session_id);

	return ErrorCode::None;
}

void PacketProcessor::ReqRemoveSessionRoomLeaveProcess(Session* session, uint32_t session_id, Room* room, uint32_t room_id)
{
	UserLeaveRoom_(session_id, room_id);
	session->SetRoomId(0);

	room->NtfRoomUserLeave(session_id);

	if (room->IsGameStarted() && room->IsPlayer(session_id))
	{
		NotifyOthersEndGame(room, room_id, session_id);
	}

	if (room->IsMatched() && room->IsPlayer(session_id))
	{
		room->EndMatch();
	}

	if (room->IsAdmin(session_id))
	{
		ChangeAdminProcess(room);
	}
}

void PacketProcessor::NotifyOthersEndGame(Room* room, uint32_t room_id, uint32_t session_id)
{
	auto opponent_id = room->GetOpponentPlayer(session_id);
	packet_sender_.NtfGameOver(opponent_id, 1);

	room->NtfGameOverView(opponent_id, session_id);

	std::print("{}번 방 게임 종료. {} 승리 ,{} 패배\n", room_id, GetSession_(opponent_id)->GetUserId(), GetSession_(session_id)->GetUserId());
}

void PacketProcessor::ChangeAdminProcess(Room* room)
{
	room->ChangeAdmin();

	if (room->IsEmpty() == false)
	{
		auto new_admin_id = room->GetAdminId();
		packet_sender_.ResYouAreRoomAdmin(new_admin_id);

		room->NtfNewRoomAdmin();
	}
}

void PacketProcessor::TimerCheck()
{
	time_count_++;
	
	auto room_list = GetAllRooms_();
	for (auto room : room_list)
	{
		room->TimerCheck(time_count_);
	}
}