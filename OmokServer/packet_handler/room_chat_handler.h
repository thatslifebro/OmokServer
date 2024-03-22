#pragma once

#include <functional>

#include "../room.h"
#include "../error_code.h"
#include "../session.h"

class RoomChatHandler
{
public:
    void Init(std::function<Room* (uint32_t room_id)> GetRoom,
        std::function<Session* (uint32_t session_id)> GetSession);

    ErrorCode HandleRequest(Packet packet);

private:
    PacketSender packet_sender_;

    std::string ExtractPacketData(Packet packet);
    void ProcessRequest(uint32_t session_id, std::string chat, Room* room);
    ErrorCode CheckError(uint32_t session_id, Session* session);

    std::function<Room* (uint32_t room_id)> GetRoom_;

    std::function<Session* (uint32_t session_id)>  GetSession_;
};