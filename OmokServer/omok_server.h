#pragma once
#include <print>

#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/ParallelSocketAcceptor.h>

#include "packet_processor.h"
#include "db_processor.h"

constexpr Poco::UInt16 SERVER_PORT = 32451;
constexpr uint32_t BUFFER_SIZE = 8096 * 10;

class OmokServer
{
public:
	RoomManager room_manager_;
	GameRoomManager game_room_manager_;
	PacketProcessor packet_processor_;
	DBProcessor db_processor_;
	UserInfo user_info_;

	void Init();
	void Start();

private:
	Poco::Net::ServerSocket server_socket_;

	void PacketProcessorStart();
	void DBProcessorStart();
};