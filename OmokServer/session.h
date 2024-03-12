#pragma once
#include <functional>
#include <unordered_map>
#include <print>

#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/ParallelSocketAcceptor.h"

#include "packet_queue.h"
#include "session_manager.h"
#include "match_manager.h"
#include "room_manager.h"
#include "packet_sender.h"

class Session
{
public:
	uint32_t session_id_;

	std::string user_id_;
	bool is_logged_in_ = false;

	uint16_t room_id_ = 0;
	
	bool is_matching_ = false;
	bool is_ready_ = false;
	uint32_t game_id_ = 0;

	Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor);

	~Session();

	void onReadable(ReadableNotification* pNotification);

	void SavePacket(std::shared_ptr<char[]> buffer, uint32_t length);

	void SendPacket(std::shared_ptr<char[]> buffer, int length);

	void Login(const std::string& user_id);

private:
	Poco::Net::StreamSocket socket_;
	Poco::Net::SocketReactor& reactor_;
	std::string peer_address_;
};