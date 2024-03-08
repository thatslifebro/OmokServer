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
	uint16_t session_room_id_ = 0;
	std::string user_id_;
	uint32_t session_id_;
	bool is_logged_in_ = false;
	uint32_t game_room_id_ = 0;
	bool is_matching_ = false;
	bool is_ready_ = false;

	Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor);

	~Session();

	void onReadable(ReadableNotification* pNotification);

	void onShutdown(ShutdownNotification* pNotification);

	void SavePacket(char* buffer);

	void SendPacket(char* buffer, int length);

	void Login(std::string user_id);

private:
	Poco::Net::StreamSocket socket_;
	Poco::Net::SocketReactor& reactor_;
	std::string peer_address_;
};