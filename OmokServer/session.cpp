#include "session.h"

Session::Session(StreamSocket& socket, SocketReactor& reactor) : socket_(socket), reactor_(reactor)
{
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
	Packet packet;
	char size[2] = { buffer[0], buffer[1] };
	packet.packet_body_size_ = *reinterpret_cast<uint16_t*>(size) - PacketInfo::header_size_;
	char id[2] = { buffer[2], buffer[3] };
	packet.packet_id_ = *reinterpret_cast<uint16_t*>(id);
	packet.packet_body_ = buffer + 4;
	packet.socket_ = &socket_;

	PacketQueue pq;
	pq.Save(packet);
}