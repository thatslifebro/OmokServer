#pragma once
#include <functional>

#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/ParallelSocketAcceptor.h"

#include "packet_buffer_manager.h"


class Session
{
public:
	Session(StreamSocket& socket, SocketReactor& reactor);

	void onReadable(ReadableNotification* pNotification);

	void onShutdown(ShutdownNotification* pNotification);

	~Session();

	std::function<bool(char*, uint32_t)> WriteData;

private:
	StreamSocket socket_;
	SocketReactor& reactor_;
	std::string peer_address_;
	
};