#include "omok_server.h"

void OmokServer::Init()
{
	Poco::Net::ServerSocket server_socket(SERVER_PORT);
	server_socket_ = server_socket;

	room_manager_.SendPacket = [&](uint32_t session_id, std::shared_ptr<char[]> buffer, int length)
		{
			auto session = session_manager_.GetSession(session_id);
			session->SendPacket(buffer, length);
		};

	room_manager_.GetUserId = [&] (uint32_t session_id)
		{
			auto session = session_manager_.GetSession(session_id);
			return session->user_id_;
		};
	room_manager_.Init();

	packet_processor_.Init();
	db_processor_.Init();
}

void OmokServer::Start()
{
	std::jthread packet_processor_thread(&OmokServer::PacketProcessorStart, this);
	std::jthread db_processor_thread(&OmokServer::DBProcessorStart, this);

	Poco::Net::SocketReactor reactor;
	Poco::Net::ParallelSocketAcceptor<Session, Poco::Net::SocketReactor> acceptor(server_socket_, reactor);
	reactor.run();
}

void OmokServer::PacketProcessorStart()
{
	while (true)
	{
		auto process = packet_processor_.ProcessPacket();
		if (!process)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		// 1초마다 타이머 체크
		auto now = clock();
		if (now - start_time_ >= 1000)
		{
			start_time_ = now;
			packet_processor_.TimerCheck();
		}
	}
}

void OmokServer::DBProcessorStart()
{
	while (true)
	{
		auto process = db_processor_.ProcessDB();
		if (!process)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}