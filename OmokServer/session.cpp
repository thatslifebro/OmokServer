#include "session.h"

Session::Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor) : socket_(socket), reactor_(reactor)
{
	//Session ¿˙¿Â
	SessionManager session_manager;
	session_id_= session_manager.AddSession(this);

	reactor_.addEventHandler(socket_, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));
	reactor_.addEventHandler(socket_, Poco::Observer<Session, ShutdownNotification>(*this, &Session::onShutdown));

	peer_address_ = socket_.peerAddress().toString();
	std::print("Connection from {}\n", peer_address_);
}

void Session::onReadable(ReadableNotification* pNotification)
{
	pNotification->release();
	try
	{
		char buffer[1024] = { 0, };
		int n = socket_.receiveBytes(buffer, sizeof(buffer));

		if (n > 0) {
			std::print("Received from client\n");
			SavePacket(buffer);
		}
		else {
			socket_.shutdown();
			delete this;
		}
	}
	catch (Poco::Exception& exc)
	{
		socket_.shutdown();
		delete this;
	}
}

void Session::onShutdown(ShutdownNotification* pNotification)
{
	pNotification->release();
}

Session::~Session()
{
	reactor_.removeEventHandler(socket_, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));
	std::print("Connection from {} closed\n", peer_address_);
	reactor_.removeEventHandler(socket_, Poco::Observer<Session, ShutdownNotification
	>(*this, &Session::onShutdown));
}

void Session::SavePacket(char* buffer)
{
	PacketQueue pq;
	pq.Save(buffer, session_id_);
}

void Session::SendPacket(char* buffer, int length)
{
	socket_.sendBytes(buffer, length);
}