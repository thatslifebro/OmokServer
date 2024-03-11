#include "omok_server.h"

void OmokServer::Init()
{
	Poco::Net::ServerSocket server_socket(SERVER_PORT);
	server_socket_ = server_socket;

	room_manager_.Init();
	game_room_manager_.Init();
	packet_processor_.Init();
	db_processor_.Init();
	user_info_.Init();
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
		auto process =  packet_processor_.ProcessPacket();
		if (!process)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
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