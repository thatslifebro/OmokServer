#include "omok_server.h"

void OmokServer::Init(flags::args args)
{
	ParseConfig(args);

	Poco::Net::ServerSocket server_socket(server_port_);
	server_socket_ = server_socket;

	room_manager_.Init(max_room_num_,
		[&](uint32_t session_id, std::shared_ptr<char[]> buffer, int length)
		{
			auto session = session_manager_.GetSession(session_id);
			session->SendPacket(buffer, length);
		},
		[&](uint32_t session_id)
		{
			auto session = session_manager_.GetSession(session_id);
			return session->GetUserId();
		});
	packet_processor_.PopAndGetPacket = [&]() -> const Packet& {
		return packet_queue_.PopAndGetPacket();
		};

	packet_processor_.Init();
	db_processor_.Init();
}

void OmokServer::Start()
{
	std::jthread packet_processor_thread(&OmokServer::PacketProcessorStart, this);
	std::jthread db_processor_thread(&OmokServer::DBProcessorStart, this);

	Poco::Net::SocketReactor reactor;
	Poco::Net::ParallelSocketAcceptor<Session, Poco::Net::SocketReactor> acceptor(server_socket_, reactor);
	acceptor.SavePacket = [&](std::shared_ptr<char[]> buffer, uint32_t length, uint32_t session_id)
	{
		packet_queue_.Save(buffer, length, session_id);
	};
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

void OmokServer::ParseConfig(flags::args args)
{
	const auto server_port = args.get<uint16_t>("port");

	if (!server_port)
	{
		server_port_ = SERVER_PORT;
	}
	else
	{
		server_port_ = *server_port;
	}

	const auto max_room_num = args.get<uint16_t>("room_num");
	if (!max_room_num)
	{
		max_room_num_ = MAX_ROOM_NUM;
	}
	else
	{
		max_room_num_ = *max_room_num;
	}
}