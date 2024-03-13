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

Session::~Session()
{
	// �̺�Ʈ �ڵ鷯 ����
	reactor_.removeEventHandler(socket_, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));

	// ���� �Ŵ������� ����
	SessionManager session_manager;
	session_manager.RemoveSession(session_id_);

	// ���� ���� ��
	RoomManager room_manager;
	PacketSender packet_sender;

	if (room_id_ == 0)
	{
		return;
	}
	room_manager.RemoveSession(session_id_, room_id_);

	auto room = room_manager.GetRoom(room_id_);
	
	if (room->IsAdmin(session_id_) || room->IsOpponent(session_id_))
	{
		if (room->IsGameStarted())
		{
			auto game = room->GetGame();
			auto winner_id = game->GetOpponentId(session_id_);
			auto winner_session = session_manager.GetSession(winner_id);

			//���� ��� ����
			packet_sender.NtfGameOver(winner_session, 1);

			room->EndGame();
		}
	}

	if (room->IsMatched())
	{
		room->CancelMatch();
	}

	// ���� ����
	auto room_session_ids = room_manager.GetSessionList(room_id_);
	packet_sender.NtfRoomUserLeave(room_session_ids, this);
	// ������ �����ٸ�
	if (room->ChangeAdmin(session_id_))
	{
		auto admin_session = session_manager.GetSession(room->GetAdminId());
		packet_sender.NtfRoomAdmin(admin_session);
		packet_sender.NtfNewRoomAdmin(room_session_ids, admin_session);
	}
	
	std::print("Connection from {} closed\n", peer_address_);
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

void Session::Login(const std::string& user_id)
{
	is_logged_in_ = true;
	user_id_ = user_id;
}