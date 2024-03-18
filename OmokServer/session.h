#pragma once
#include <functional>
#include <unordered_map>
#include <print>

#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/ParallelSocketAcceptor.h"

#include "packet_queue.h"
#include "session_manager.h"
#include "room_manager.h"
#include "packet_sender.h"

class Session
{
public:
	Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor, std::function<void(std::shared_ptr<char[]>, uint32_t, uint32_t)> SavePacket);

	~Session();

	void onReadable(ReadableNotification* pNotification);

	//void SavePacket(std::shared_ptr<char[]> buffer, uint32_t length);

	void SendPacket(std::shared_ptr<char[]> buffer, int length);

	bool IsLoggedIn() { return is_logged_in_; }

	void SetLoggedIn(bool is_logged_in) { is_logged_in_ = is_logged_in; }

	void SetUserId(std::string user_id) { user_id_ = user_id; }

	std::string GetUserId() { return user_id_; }

	void SetRoomId(uint16_t room_id) { room_id_ = room_id; }

	uint16_t GetRoomId() { return room_id_; }

	bool IsInRoom() { return room_id_ != 0; }

	void SetSessionId(uint32_t session_id) { session_id_ = session_id; }

	uint32_t GetSessionId() { return session_id_; }

	std::function<void(std::shared_ptr<char[]>, uint32_t, uint32_t)> SavePacket;

private:
	SessionManager session_manager_;
	RoomManager room_manager_;
	PacketSender packet_sender_;

	uint32_t session_id_;
	std::string user_id_;
	bool is_logged_in_ = false;
	uint16_t room_id_ = 0;

	Poco::Net::StreamSocket socket_;
	Poco::Net::SocketReactor& reactor_;
	std::string peer_address_;

	void LeaveRoom();
};