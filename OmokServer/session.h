#pragma once
#include <functional>
#include <unordered_map>

#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/ParallelSocketAcceptor.h"

#include "packet_queue.h"
#include "session_manager.h"

class Session
{
public:
	Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor);
	void onReadable(ReadableNotification* pNotification);
	void onShutdown(ShutdownNotification* pNotification);
	~Session();
	void SavePacket(char* buffer);
	void SendPacket(char* buffer, int length);

private:
	Poco::Net::StreamSocket socket_;
	Poco::Net::SocketReactor& reactor_;
	std::string peer_address_;
	int session_id_;
	
};