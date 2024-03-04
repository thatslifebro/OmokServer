#pragma once
#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/ParallelSocketAcceptor.h>
#include "packet_processor.h"
#include "poco_session.h"
#include "packet_info.h"

constexpr Poco::UInt16 SERVER_PORT = 32452;
constexpr uint32_t BUFFER_SIZE = 8096 * 10;

class OmokServer
{
public:
	void Init();
	void PacketProcess();
};