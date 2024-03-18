#include "session.h"

Session::Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor) : socket_(socket), reactor_(reactor)
{
	// session_id_�� Session ����
	session_id_= session_manager_.AddSession(this);

	packet_sender_.Init([&](uint32_t session_id, std::shared_ptr<char[]> buffer, int length) {
		auto session = session_manager_.GetSession(session_id);
		if (session == nullptr)
		{
			return;
		}
		session->SendPacket(buffer, length);
		});

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

	// room ���� ����
	LeaveRoom();

	// ���� �Ŵ������� ����
	session_manager.RemoveSession(session_id_);

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
	if (room_id_ == 0)
	{
		return;
	}
	room_manager_.RemoveUser(session_id_, room_id_);

	// �������� ��
	auto room = room_manager_.GetRoom(room_id_);

	if (room->IsPlayer(session_id_))
	{
		auto opponent_id = room->GetOpponentPlayer(session_id_);
		packet_sender_.NtfGameOver(opponent_id, 1);
	}

	// ��Ī ���� �� ��
	if (room->IsMatched())
	{
		room->EndMatch();
	}

	// ���� ����
	room->NtfRoomUserLeave(session_id_);

	// ������ ��
	if (room->IsAdmin(session_id_))
	{
		room->ChangeAdmin();

		auto admin_session_id = room->GetAdminId();

		packet_sender_.ResYouAreRoomAdmin(admin_session_id);
		room->NtfNewRoomAdmin();
	}
}