#pragma once
#include <print>

#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/ParallelSocketAcceptor.h>

#include "packet_processor.h"
#include "db_processor.h"
#include "timeout_processor.h"

class OmokServer
{
public:
	RoomManager room_manager_;
	PacketProcessor packet_processor_;
	DBProcessor db_processor_;
	TimeoutProcessor timeout_processor_;

	void Init();
	void Start();

private:
	Poco::Net::ServerSocket server_socket_;

	void PacketProcessorStart();
	void DBProcessorStart();
	void TimeoutProcessorStart();
};