#include "session.h"

Session::Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor, std::function<void(std::shared_ptr<char[]>, uint32_t, uint32_t)> SavePacket) : socket_(socket), reactor_(reactor), SavePacket(SavePacket)
{
	// session_id_로 Session 관리
	session_id_= session_manager_.AddSession(this);

	packet_sender_.Init([&](uint32_t session_id, std::shared_ptr<char[]> buffer, int length) {
		auto session = session_manager_.GetSession(session_id);
		if (session == nullptr)
		{
			return;
		}
		session->SendPacket(buffer, length);
		});

	// 이벤트 헨들러 등록
	reactor_.addEventHandler(socket_, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));

	// 클라이언트 주소 저장
	peer_address_ = socket_.peerAddress().toString();
	std::print("Connection from {}\n", peer_address_);
}

Session::~Session()
{
	SessionManager session_manager;

	// 이벤트 핸들러 제거
	reactor_.removeEventHandler(socket_, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));

	// room 에서 제거
	LeaveRoom();

	// 세션 매니저에서 제거
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

		if (n > 0)
		{
			std::print("Received {} bytes from {}\n", n, peer_address_);
			SavePacket(buffer, n, session_id_); // 변경
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

//void Session::SavePacket(std::shared_ptr<char[]> buffer, uint32_t length)
//{
//	PacketQueue pq;
//	pq.Save(buffer, length, session_id_);
//}

void Session::SendPacket(std::shared_ptr<char[]> buffer, int length)
{
	socket_.sendBytes(buffer.get(), length);
}

void Session::LeaveRoom()
{
	if (IsInRoom() == false)
	{
		return;
	}

	auto room_id = room_id_;
	auto room = room_manager_.GetRoom(room_id);

	room_manager_.RemoveUser(session_id_, room_id);
	room_id_ = 0;

	std::print("UserId : {} 가 방에서 나감.\n", user_id_);

	room->NtfRoomUserLeave(session_id_);

	if (room->IsGameStarted() && room->IsPlayer(session_id_))
	{
		auto opponent_id = room->GetOpponentPlayer(session_id_);
		packet_sender_.NtfGameOver(opponent_id, 1);
		std::print("{}번 방 게임 종료. {} 승리 ,{} 패배\n", room_id, session_manager_.GetSession(opponent_id)->GetUserId(), user_id_);
	}

	if (room->IsMatched())
	{
		room->EndMatch();
	}

	if (room->IsAdmin(session_id_))
	{
		room->ChangeAdmin();

		auto new_admin_id = room->GetAdminId();
		packet_sender_.ResYouAreRoomAdmin(new_admin_id);

		room->NtfNewRoomAdmin();
	}

}