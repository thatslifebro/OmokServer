#pragma once
#include <functional>
#include <unordered_map>

#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/ParallelSocketAcceptor.h"

#include "packet_queue.h"
#include "packet_info.h"


class Session
{
public:
	Session(StreamSocket& socket, SocketReactor& reactor);

	void onReadable(ReadableNotification* pNotification);

	void onShutdown(ShutdownNotification* pNotification);

	~Session();

	void SavePacket(char* buffer);

private:
	StreamSocket socket_;
	SocketReactor& reactor_;
	std::string peer_address_;
	
};