#include "session.h"

Session::Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor) : socket_(socket), reactor_(reactor)
{
	// session_id_로 Session 관리
	SessionManager session_manager;
	session_id_= session_manager.AddSession(this);

	// 이벤트 헨들러 등록
	reactor_.addEventHandler(socket_, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));

	// 클라이언트 주소 저장
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
	// 이벤트 핸들러 제거
	reactor_.removeEventHandler(socket_, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));

	// 세션 매니저에서 제거
	SessionManager session_manager;
	session_manager.RemoveSession(session_id_);

	// 게임 중일 때
	if (game_room_id_ != 0)
	{
		// 게임 종료
		GameRoomManager game__room_manager;
		auto opponent_session_id = game__room_manager.GetGameRoom(game_room_id_)->GetOpponentId(session_id_);
		auto opponent_session = session_manager.GetSession(opponent_session_id);
		game__room_manager.GameEnd(game_room_id_);
		
		// 게임 종료 알리기
		PacketSender packet_sender;
		packet_sender.NtfGameOver(opponent_session, -1);

		// 상대방 초기화
		opponent_session->game_room_id_ = 0;
		opponent_session->is_matching_ = false;
		opponent_session->is_ready_ = false;
	}

	// 로그아웃
	if (is_logged_in_)
	{
		UserInfo user_info;
		user_info.Logout(user_id_);
	}

	// 방에 입장 중일 때
	if (room_id_ != 0)
	{
		// 방에서 제거
		RoomManager room_manager;
		room_manager.RemoveSession(session_id_, room_id_);

		auto room_session_ids = room_manager.GetSessionList(room_id_);

		// 전파
		PacketSender packet_sender;
		packet_sender.NtfRoomUserLeave(room_session_ids, this);
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