#include "omok_server.h"

void OmokServer::Init()
{
	Poco::Net::ServerSocket server_socket(SERVER_PORT);
	server_socket_ = server_socket;

	room_manager_.Init();
	game_room_manager_.Init();
}

void OmokServer::Start()
{
	Poco::Net::SocketReactor reactor;
	Poco::Net::ParallelSocketAcceptor<Session, Poco::Net::SocketReactor> acceptor(server_socket_, reactor);
	std::jthread packet_processor_thread(&OmokServer::PacketProcessorStart, this);
	reactor.run();
}

void OmokServer::PacketProcessorStart()
{
	PacketProcessor packet_processor;
	packet_processor.Init();

	while (true)
	{
		auto process =  packet_processor.ProcessPacket();
		if (!process)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}
