#pragma once
#include <print>

#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/ParallelSocketAcceptor.h>

#include "packet_processor.h"
#include "db_processor.h"
#include "session_manager.h"
#include "room_manager.h"
#include "packet_queue.h"
#include "db_packet_queue.h"
#include "flags.h"

class OmokServer
{
public:
	void Init(flags::args args);
	void Start();

private:
	PacketProcessor packet_processor_;
	DBProcessor db_processor_;
	SessionManager session_manager_;
	RoomManager room_manager_;
	PacketQueue packet_queue_;
	DBPacketQueue db_packet_queue_;

	Poco::Net::ServerSocket server_socket_;
	Poco::Net::SocketReactor reactor_;

	clock_t start_time_ = clock();

	void PacketProcessorStart();
	void DBProcessorStart();

	std::tuple<uint16_t, uint16_t> ParseConfig(flags::args args);

	uint16_t ParseConfigPort(flags::args args);
	uint16_t ParseConfigRoomNum(flags::args args);

	void InitRoomManager(uint16_t room_num);
	void InitPacketProcessor();
	void InitDBProcessor();

	void InitAcceptor(Poco::Net::ParallelSocketAcceptor<Session, Poco::Net::SocketReactor>& acceptor);
};