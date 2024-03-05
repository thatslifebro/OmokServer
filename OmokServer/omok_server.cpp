#include "omok_server.h"

void OmokServer::Init()
{
	//통신 버퍼 초기화
	PacketBufferManager packet_buffer_manager;
	packet_buffer_manager.Init(BUFFER_SIZE, PacketInfo::header_size_, PacketInfo::max_packet_size_);
}

void OmokServer::Start()
{
	//패킷 처리 스레드
	std::jthread packet_process_thread(&OmokServer::PacketProcess, this);
	std::print("PacketProcess thread started\n");
	//Poco paraller reactor 이용한 서버
	std::print("Server started\n");
	Poco::Net::SocketReactor reactor;
	Poco::Net::ServerSocket server_socket(SERVER_PORT);
	Poco::Net::ParallelSocketAcceptor<Session, Poco::Net::SocketReactor> acceptor(server_socket, reactor);
	reactor.run();
	std::print("Server running\n");
}


void OmokServer::PacketProcess()
{
	PacketBufferManager packet_buffer_manager;

	PacketProcessor packet_processor;

	// todo : C++20의 coroutine 사용하여 무한으로 돌지 않고 버퍼가 차있을 때 다시 실행하도록 할 수 있을 것 같다.
	while (true)
	{
		auto packet = packet_buffer_manager.Read();
		if (packet != nullptr) {
			std::print("ProcessPacket\n");
			packet_processor.ProcessPacket(packet);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}

OmokServer::~OmokServer()
{
	PacketBufferManager packet_buffer_manager;
	packet_buffer_manager.ClearBuffer();
}