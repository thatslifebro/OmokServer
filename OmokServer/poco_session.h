#pragma once
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

private:
	StreamSocket socket_;
	SocketReactor& reactor_;
	std::string peer_address_;
	PacketBufferManager* packet_manager_;
};