#include "session.h"

Session::Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor) : socket_(socket), reactor_(reactor)
{
	// session_id_�� Session ����
	SessionManager session_manager;
	session_id_= session_manager.AddSession(this);

	// �̺�Ʈ ��鷯 ���
	reactor_.addEventHandler(socket_, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));

	// Ŭ���̾�Ʈ �ּ� ����
	peer_address_ = socket_.peerAddress().toString();
	std::print("Connection from {}\n", peer_address_);
}


Session::~Session()
{
	SessionManager session_manager;

	// �̺�Ʈ �ڵ鷯 ����
	reactor_.removeEventHandler(socket_, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));

	// ���� �Ŵ������� ����
	session_manager.RemoveSession(session_id_);

	// room ���� ����
	LeaveRoom();

	std::print("Connection from {} closed\n", peer_address_);
}

void Session::onReadable(ReadableNotification* pNotification)
{
	pNotification->release();
	try
	{
		std::shared_ptr<char[]> buffer(new char[MAX_PACKET_SIZE]);
		auto n = socket_.receiveBytes(buffer.get(), MAX_PACKET_SIZE);
		std::print("Received {} bytes from {}\n", n, peer_address_);

		if (n > 0)
		{
			SavePacket(buffer, n);
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

void Session::SavePacket(std::shared_ptr<char[]> buffer, uint32_t length)
{
	PacketQueue pq;
	pq.Save(buffer, length, session_id_);
}

void Session::SendPacket(std::shared_ptr<char[]> buffer, int length)
{
	socket_.sendBytes(buffer.get(), length);
}

void Session::LeaveRoom()
{
	SessionManager session_manager;
	RoomManager room_manager;
	PacketSender packet_sender;

	if (room_id_ == 0)
	{
		return;
	}
	room_manager.RemoveSession(session_id_, room_id_);

	// �������� ��
	auto room = room_manager.GetRoom(room_id_);

	auto opponent_id = room->PlayerLeave(session_id_);
	if (opponent_id != 0)
	{
		auto opponent_session = session_manager.GetSession(opponent_id);
		packet_sender.NtfGameOver(opponent_session, 1);
	}

	// ��Ī ���� �� ��
	if (room->IsMatched())
	{
		room->CancelMatch();
	}

	// ���� ����
	auto room_session_ids = room_manager.GetSessionList(room_id_);
	packet_sender.NtfRoomUserLeave(room_session_ids, this);

	// ������ ��
	if (room->IsAdmin(session_id_))
	{
		room->ChangeAdmin();

		auto admin_session = session_manager.GetSession(room->GetAdminId());
		if (admin_session == nullptr)
		{
			return;
		}

		packet_sender.NtfRoomAdmin(admin_session);
		packet_sender.NtfNewRoomAdmin(room_session_ids, admin_session);
	}
}