#include "session.h"

Session::Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor,
	std::function<void(std::shared_ptr<char[]>, uint32_t, uint32_t)> SaveByteArray,
	std::function<void(Packet packet)> SavePacket,
	std::function<int(Session* session)> AddSession,
	std::function<Session* (uint32_t session_id)> GetSession,
	std::function<Room* (uint32_t room_id)> GetRoom,
	std::function<void(uint32_t session_id, uint32_t room_id)> RemoveUser)
	: socket_(socket), reactor_(reactor),
	SaveByteArray_(SaveByteArray), SavePacket_(SavePacket),
	AddSession_(AddSession), GetSession_(GetSession),
	GetRoom_(GetRoom), RemoveUser_(RemoveUser)
{
	session_id_ = AddSession_(this);

	packet_sender_.InitSendPacketFunc([&](uint32_t session_id, char* buffer, int length) {
		auto session = GetSession_(session_id);
		if (session == nullptr)
		{
			return;
		}
		session->SendPacket(buffer, length);
		});

	reactor_.addEventHandler(socket_, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));

	peer_address_ = socket_.peerAddress().toString();
	std::print("Connection from {}\n", peer_address_);
}

Session::~Session()
{
	reactor_.removeEventHandler(socket_, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));

	if (IsInRoom())
	{
		LeaveRoom();
	}

	RemoveSession();

	std::print("Connection from {} closed\n", peer_address_);
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
			std::print("Received {} bytes from {}\n", n, peer_address_);
			SaveByteArray_(buffer, n, session_id_);
		}
		else
		{
			socket_.shutdown();
			delete this;
		}
	}
	catch (Poco::Exception& e)
	{
		e.displayText();
		socket_.shutdown();
		delete this;
	}
}

void Session::SendPacket(char* buffer, int length)
{
	socket_.sendBytes(buffer, length);
}

void Session::LeaveRoom()
{
	auto room_id = room_id_;
	auto room = GetRoom_(room_id);

	RemoveUser_(session_id_, room_id);
	room_id_ = 0;

	std::print("UserId : {} 가 방에서 나감.\n", user_id_);

	room->NtfRoomUserLeave_(session_id_);

	if (room->IsGameStarted() && room->IsPlayer(session_id_))
	{
		auto opponent_id = room->GetOpponentPlayer(session_id_);
		packet_sender_.NtfGameOver(opponent_id, 1);
		std::print("{}번 방 게임 종료. {} 승리 ,{} 패배\n", room_id, GetSession_(opponent_id)->GetUserId(), user_id_);
	}

	if (room->IsMatched())
	{
		room->EndMatch();
	}

	if (room->IsAdmin(session_id_))
	{
		room->ChangeAdmin();

		if (room->IsEmpty() == false)
		{
			auto new_admin_id = room->GetAdminId();
			packet_sender_.ResYouAreRoomAdmin(new_admin_id);

			room->NtfNewRoomAdmin_();
		}
	}
}

void Session::RemoveSession()
{
	Packet packet;
	packet.SetPacketId(static_cast<uint16_t>(PacketId::ReqRemoveSession));
	packet.SetPacketSize(PacketHeader::HEADER_SIZE);
	packet.SetSessionId(session_id_);

	SavePacket_(packet);
}