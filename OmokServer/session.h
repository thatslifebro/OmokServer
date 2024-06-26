#pragma once
#include <functional>
#include <print>

#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/ParallelSocketAcceptor.h"

#include "packet.h"

class Session
{
public:
	Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor,
		std::function<void(std::shared_ptr<char[]>, uint32_t, uint32_t)> PushPacketFromData,
		std::function<void(Packet packet)> PushPacket);

	~Session();

	void onReadable(ReadableNotification* pNotification);

	void SendPacket(char* buffer, int length);

	bool IsLoggedIn() const { return is_logged_in_; }

	void SetLoggedIn(bool is_logged_in) { is_logged_in_ = is_logged_in; }

	void SetUserId(std::string user_id) { user_id_ = user_id; }

	std::string GetUserId() { return user_id_; }

	void SetRoomId(uint32_t room_id) { room_id_ = room_id; }

	uint32_t GetRoomId() const { return room_id_; }

	bool IsInRoom() const { return room_id_ != 0; }

	void SetSessionId(uint32_t session_id) { session_id_ = session_id; }

	uint32_t GetSessionId() const { return session_id_; }

private:
	HANDLE h_event_object_;

	uint32_t session_id_;
	std::string user_id_;
	bool is_logged_in_ = false;
	uint32_t room_id_ = 0;

	Poco::Net::StreamSocket socket_;
	Poco::Net::SocketReactor& reactor_;
	std::string peer_address_;

	std::function<void(std::shared_ptr<char[]>, uint32_t, uint32_t)> PushPacketFromData_;
	std::function<void(Packet packet)> PushPacket_;

	void AddSessionReq();
	void RemoveSessionReq();
};