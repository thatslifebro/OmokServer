#include "session.h"

Session::Session(StreamSocket& socket, SocketReactor& reactor) : socket_(socket), reactor_(reactor)
{
	WriteData = [&](char* data, uint32_t size) {
		PacketBufferManager packet_manager;
		return packet_manager.Write(data, size);
	};

	reactor_.addEventHandler(socket_, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));
	peer_address_ = socket_.peerAddress().toString();
	std::print("Connection from {}\n",peer_address_);

	reactor_.addEventHandler(socket_, Poco::Observer<Session, ShutdownNotification>(*this, &Session::onShutdown));
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

			if (WriteData(buffer, n) == false) {
				std::print("Fail PacketBuffer->Write\n");
			}
			std::print("success!\n");
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
