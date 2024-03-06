#include "omok_server.h"

void OmokServer::Init()
{
	// todo : init
}

void OmokServer::Start()
{
	SocketReactor reactor;
	Poco::Net::ServerSocket server_socket(SERVER_PORT);
	Poco::Net::ParallelSocketAcceptor<Session, Poco::Net::SocketReactor> acceptor(server_socket, reactor);
	std::jthread packet_processor_thread(&OmokServer::PacketProcessorStart, this);
	reactor.run();
}


void OmokServer::PacketProcessorStart()
{
	PacketProcessor packet_processor;

	// todo : C++20의 coroutine 사용하여 무한으로 돌지 않고 버퍼가 차있을 때 다시 실행하도록 할 수 있을 것 같다.
	while (true)
	{
		auto process =  packet_processor.ProcessPacket();
		if (!process)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}
