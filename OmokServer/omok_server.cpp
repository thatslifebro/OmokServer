#include "omok_server.h"

void OmokServer::Init() {
	//��� ���� �ʱ�ȭ
	PacketBufferManager* packet_buffer_manager = PacketBufferManager::GetInstance();
	packet_buffer_manager->Init(BUFFER_SIZE, PacketInfo::header_size_, PacketInfo::max_packet_size_);

	//��Ŷ ó�� ������
	std::jthread packet_process_thread(&OmokServer::PacketProcess, this);

	//Poco paraller reactor �̿��� ����
	Poco::Net::SocketReactor reactor;
	Poco::Net::ServerSocket server_socket(SERVER_PORT);
	Poco::Net::ParallelSocketAcceptor<Session, Poco::Net::SocketReactor> acceptor(server_socket, reactor);
	reactor.run();
}


void OmokServer::PacketProcess()
{
	PacketBufferManager* packet_buffer_manager = PacketBufferManager::GetInstance();

	PacketProcessor packet_processor;

	// todo : C++20�� coroutine ����Ͽ� �������� ���� �ʰ� ���۰� ������ �� �ٽ� �����ϵ��� �� �� ���� �� ����.
	while (true)
	{
		auto packet = packet_buffer_manager->Read();
		if (packet != nullptr) {
			std::cout << "ProcessPacket" << std::endl;
			packet_processor.ProcessPacket(packet);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}