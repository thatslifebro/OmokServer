#pragma once

#include <functional>

#include "../room.h"
#include "../error_code.h"
#include "../session.h"

class RoomEnterHandler
{
public:
    void Init(std::function<void(uint32_t session_id, uint32_t room_id)> UserEnterRoom,
        std::function<Room* (uint32_t room_id)> GetRoom,
        std::function<Session* (uint32_t session_id)> GetSession);

    ErrorCode HandleRequest(Packet packet);

private:
    PacketSender packet_sender_;

    uint32_t ExtractPacketData(Packet packet);
    void ProcessRequest(uint32_t session_id, Session* session, std::string user_id, uint32_t room_id, Room* room);
    ErrorCode CheckError(uint32_t session_id, Session* session, std::string user_id, Room* room);

	std::function<void(uint32_t session_id, uint32_t room_id)> UserEnterRoom_;
	std::function<Room* (uint32_t room_id)> GetRoom_;

	std::function<Session* (uint32_t session_id)>  GetSession_;
};