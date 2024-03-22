#pragma once

#include <functional>

#include "../room.h"
#include "../error_code.h"
#include "../session.h"

class MatchResHandler
{
public:
    void Init(std::function<Room* (uint32_t room_id)> GetRoom,
        std::function<Session* (uint32_t session_id)> GetSession);

    ErrorCode HandleRequest(Packet packet, uint32_t time_count);

private:
    PacketSender packet_sender_;

    bool ExtractPacketData(Packet packet);
    void ProcessRequest(uint32_t session_id, Session* session, uint32_t admin_id, bool accept, Room* room, uint32_t time_count);
    ErrorCode CheckError(uint32_t session_id, uint32_t admin_id, Room* room);

    std::function<Room* (uint32_t room_id)> GetRoom_;

    std::function<Session* (uint32_t session_id)>  GetSession_;
};