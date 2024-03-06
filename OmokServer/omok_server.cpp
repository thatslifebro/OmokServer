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

	// todo : C++20�� coroutine ����Ͽ� �������� ���� �ʰ� ���۰� ������ �� �ٽ� �����ϵ��� �� �� ���� �� ����.
	while (true)
	{
		auto process =  packet_processor.ProcessPacket();
		if (!process)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}
