#pragma once

#include <functional>

#include "../room.h"
#include "../error_code.h"
#include "../session.h"

class RoomLeaveHandler
{
public:
    void Init(std::function<void(uint32_t session_id, uint32_t room_id)> UserLeaveRoom,
        std::function<Room* (uint32_t room_id)> GetRoom,
        std::function<Session* (uint32_t session_id)> GetSession);

    ErrorCode HandleRequest(Packet packet);

private:
    PacketSender packet_sender_;

    void ProcessRequest(uint32_t session_id, Session* session, uint32_t room_id, Room* room);
    ErrorCode CheckError(uint32_t session_id, Session* session);

    void NotifyOthersEndGame(Room* room, uint32_t room_id, uint32_t session_id);
    void ProcessChangeAdmin(Room* room);

    std::function<void(uint32_t session_id, uint32_t room_id)> UserLeaveRoom_;
    std::function<Room* (uint32_t room_id)> GetRoom_;

    std::function<Session* (uint32_t session_id)>  GetSession_;

};