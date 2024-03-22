#include "session.h"

Session::Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor,
	std::function<void(std::shared_ptr<char[]>, uint32_t, uint32_t)> PushPacketFromData,
	std::function<void(Packet packet)> PushPacket) : socket_(socket), reactor_(reactor),
	PushPacketFromData_(PushPacketFromData), PushPacket_(PushPacket)
{
	AddSessionReq();

	reactor_.addEventHandler(socket_, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));

	peer_address_ = socket_.peerAddress().toString();
	std::print("Connection from {}\n", peer_address_);
}

Session::~Session()
{
	socket_.shutdown();
}

void Session::onReadable(ReadableNotification* pNotification)
{
	pNotification->release();
	try
	{
		std::shared_ptr<char[]> buffer(new char[MAX_PACKET_SIZE]);
		auto n = socket_.receiveBytes(buffer.get(), MAX_PACKET_SIZE);

		if (n > 0)
		{
			//std::print("Received {} bytes from {}\n", n, peer_address_);
			PushPacketFromData_(buffer, n, session_id_);
		}
		else
		{
			RemoveSessionReq();
		}
	}
	catch (Poco::Exception&)
	{
		RemoveSessionReq();
	}
}

void Session::SendPacket(char* buffer, int length)
{
	socket_.sendBytes(buffer, length);
}

void Session::AddSessionReq()
{
	Packet packet;
	packet.SetPacketId(static_cast<uint16_t>(PacketId::ReqAddSession));
	packet.SetPacketSize(PacketHeader::HEADER_SIZE + sizeof(this));
	packet.SetSessionId(session_id_);

	auto session_addr = uint64_t(this);
	memcpy(packet.GetPacketBody(), &session_addr, sizeof(this));

	PushPacket_(packet);
}

void Session::RemoveSessionReq()
{
	reactor_.removeEventHandler(socket_, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));

	socket_.shutdownReceive();

	std::print("Connection from {} closed\n", peer_address_);

	Packet packet;
	packet.SetPacketId(static_cast<uint16_t>(PacketId::ReqRemoveSession));
	packet.SetPacketSize(PacketHeader::HEADER_SIZE);
	packet.SetSessionId(session_id_);

	PushPacket_(packet);
}