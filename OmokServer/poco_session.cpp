#include "poco_session.h"

Session::Session(StreamSocket& socket, SocketReactor& reactor) : socket_(socket), reactor_(reactor)
{
	PacketBuffer = PacketBufferManager::GetInstance();

	reactor_.addEventHandler(socket_, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));
	peer_address_ = socket_.peerAddress().toString();
	std::cout << "Connection from " << peer_address_ << std::endl;

	reactor_.addEventHandler(socket_, Poco::Observer<Session, ShutdownNotification>(*this, &Session::onShutdown));
}

void Session::onReadable(ReadableNotification* pNotification)
{
	pNotification->release();
	try
	{
		char buffer[1024] = { 0, };

		int n = socket_.receiveBytes(buffer, sizeof(buffer));

		if (n > 0)
		{

			std::cout << "Received from client " << std::endl;

			if (PacketBuffer->Write(buffer, n) == false)
			{
				std::cout << "Fail PacketBuffer->Write" << std::endl;
			}
			std::cout << "success!" << std::endl;

		}
		else
		{
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
	std::cout << "Connection from " << peer_address_ << " closed" << std::endl;
	reactor_.removeEventHandler(socket_, Poco::Observer<Session, ShutdownNotification
	>(*this, &Session::onShutdown));
}
