#include "session.h"

Session::Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor) : socket_(socket), reactor_(reactor)
{
	//Session 저장
	SessionManager session_manager;
	session_id_= session_manager.AddSession(this);

	reactor_.addEventHandler(socket_, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));
	reactor_.addEventHandler(socket_, Poco::Observer<Session, ShutdownNotification>(*this, &Session::onShutdown));

	peer_address_ = socket_.peerAddress().toString();
	std::print("Connection from {}\n", peer_address_);
}

void Session::onReadable(ReadableNotification* pNotification)
{
	pNotification->release();
	try
	{
		char buffer[1024] = { 0, };
		auto n = socket_.receiveBytes(buffer, sizeof(buffer));

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
	std::print("onShutdown\n");

}

Session::~Session()
{
	reactor_.removeEventHandler(socket_, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));
	reactor_.removeEventHandler(socket_, Poco::Observer<Session, ShutdownNotification>(*this, &Session::onShutdown));

	SessionManager session_manager;
	session_manager.RemoveSession(session_id_);

	std::print("Connection from {} closed\n", peer_address_);

	//게임 룸 처리
	GameRoomManager game__room_manager;
	PacketSender packet_sender;
	if(game_room_id_ != 0)
	{
		auto opponent_session_id = game__room_manager.GetGameRoom(game_room_id_)->GetOpponentId(session_id_);
		auto opponent_session = session_manager.GetSession(opponent_session_id);
		game__room_manager.GameEnd(game_room_id_);
		
		packet_sender.NtfGameOver(opponent_session, -1);

		opponent_session->game_room_id_ = 0;
		opponent_session->is_matching_ = false;
		opponent_session->is_ready_ = false;
	}

	// 로그아웃
	if(is_logged_in_)
	{
		UserInfo user_info;
		user_info.Logout(user_id_);
	}

	//room 처리
	if(room_id_ != 0)
	{
		RoomManager room_manager;
		room_manager.RemoveSession(session_id_, room_id_);

		auto room_session_ids = room_manager.GetSessionList(room_id_);
		packet_sender.BroadcastRoomUserLeave(room_session_ids, this);
	}
	

}

void Session::SavePacket(char* buffer)
{
	PacketQueue pq;
	pq.Save(buffer, session_id_);
}

void Session::SendPacket(char* buffer, int length)
{
	socket_.sendBytes(buffer, length);
}

void Session::Login(std::string user_id)
{
	is_logged_in_ = true;
	user_id_ = user_id;
}