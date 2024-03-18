#pragma once
#include <print>
#include <ctime>

#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/ParallelSocketAcceptor.h>

#include "packet_processor.h"
#include "db_processor.h"
#include "flags.h"

class OmokServer
{
public:
	void Init(flags::args args);
	void Start();

private:
	RoomManager room_manager_;
	SessionManager session_manager_;
	PacketProcessor packet_processor_;
	DBProcessor db_processor_;
	PacketQueue packet_queue_;

	uint16_t server_port_;
	uint16_t max_room_num_;
	Poco::Net::ServerSocket server_socket_;

	clock_t start_time_ = clock();

	void PacketProcessorStart();
	void DBProcessorStart();

	void ParseConfig(flags::args args);
};