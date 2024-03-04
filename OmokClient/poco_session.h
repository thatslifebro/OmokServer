#pragma once

#include "protobuf/OmokPacket.pb.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketNotification.h"

class Session
{
public:
	Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor);

	~Session();

	void onReadable(Poco::Net::ReadableNotification* pNf);

	void SendReqLogin(std::string id, std::string pw);

private:
	Poco::Net::StreamSocket socket_;
	Poco::Net::SocketReactor& reactor_;
	int send_count_ = 1;
	std::string peer_address_;

};