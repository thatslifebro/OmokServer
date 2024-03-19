#include "omok_server.h"

void OmokServer::Init(flags::args args)
{
	auto [server_port, room_num] = ParseConfig(args);

	Poco::Net::ServerSocket server_socket(server_port); // exception throw 발생 가능
	server_socket_ = server_socket;

	InitRoomManager(room_num);

	InitPacketProcessor();

	InitDBProcessor();
}

void OmokServer::Start()
{
	std::jthread packet_processor_thread(&OmokServer::PacketProcessorStart, this);
	std::jthread db_processor_thread(&OmokServer::DBProcessorStart, this);

	Poco::Net::SocketReactor reactor;
	Poco::Net::ParallelSocketAcceptor<Session, Poco::Net::SocketReactor> acceptor(server_socket_, reactor);

	InitAcceptor(acceptor);

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

std::tuple<uint16_t, uint16_t> OmokServer::ParseConfig(flags::args args)
{
	auto server_port = ParseConfigPort(args);
	auto max_room_num = ParseConfigRoomNum(args);
	
	return std::make_tuple(server_port, max_room_num);
}

uint16_t OmokServer::ParseConfigPort(flags::args args)
{
	const auto server_port_value = args.get<uint16_t>("port");

	uint16_t server_port;

	if (!server_port_value)
	{
		server_port = SERVER_PORT;
	}
	else
	{
		server_port = *server_port_value;
	}

	return server_port;
}

uint16_t OmokServer::ParseConfigRoomNum(flags::args args)
{
	const auto room_num_value = args.get<uint16_t>("room_num");

	uint16_t room_num;

	if (!room_num_value)
	{
		room_num = MIN_ROOM_NUM;
	}
	else
	{
		room_num = *room_num_value;
	}

	if (room_num < MIN_ROOM_NUM || room_num > MAX_ROOM_NUM)
	{
		throw std::runtime_error(std::format("Invalid room number. It should be between {} and {}\n", MIN_ROOM_NUM, MAX_ROOM_NUM));
	}

	return room_num;
}

void OmokServer::InitRoomManager(uint16_t room_num)
{
	auto SendPacket = [&](uint32_t session_id, std::shared_ptr<char[]> buffer, int length)
		{
			auto session = session_manager_.GetSession(session_id);
			session->SendPacket(buffer, length);
		};

	auto GetUserId = [&](uint32_t session_id)
		{
			auto session = session_manager_.GetSession(session_id);
			return session->GetUserId();
		};

	room_manager_.Init(room_num, SendPacket, GetUserId);
}

void OmokServer::InitPacketProcessor()
{
	auto PopAndGetPacket = [&]() { return packet_queue_.PopAndGetPacket(); };
	auto PushPacket = [&](const Packet& packet) { db_packet_queue_.PushPacket(packet); };

	packet_processor_.InitPacketQueueFunctions(PopAndGetPacket, PushPacket);

	auto AddUser = [&](uint32_t session_id, uint16_t room_id) { room_manager_.AddUser(session_id, room_id); };
	auto RemoveUser = [&](uint32_t session_id, uint16_t room_id) { room_manager_.RemoveUser(session_id, room_id); };
	auto GetRoom = [&](uint16_t room_id) { return room_manager_.GetRoom(room_id); };
	auto GetAllRooms = [&]() { return room_manager_.GetAllRooms(); };

	packet_processor_.InitRoomManagerFunctions(AddUser, RemoveUser, GetRoom, GetAllRooms);

	auto GetSession = [&](uint32_t session_id) { return session_manager_.GetSession(session_id); };

	packet_processor_.InitSessionManagerFunctions(GetSession);

	packet_processor_.Init();
}

void OmokServer::InitDBProcessor()
{
	auto PopAndGetDBPacket = [&]() { return db_packet_queue_.PopAndGetPacket(); };

	db_processor_.InitDBPacketQueueFunctions(PopAndGetDBPacket);

	auto GetSession = [&](uint32_t session_id) { return session_manager_.GetSession(session_id); };

	db_processor_.InitSessionManagerFunctions(GetSession);

	db_processor_.Init();
}

void OmokServer::InitAcceptor(Poco::Net::ParallelSocketAcceptor<Session, Poco::Net::SocketReactor>& acceptor)
{
	auto SavePacket = [&](std::shared_ptr<char[]> buffer, uint32_t length, uint32_t session_id) { packet_queue_.Save(buffer, length, session_id); };
	auto RemoveUser = [&](uint32_t session_id, uint16_t room_id) { room_manager_.RemoveUser(session_id, room_id);	};
	auto GetRoom = [&](uint16_t room_id) { return room_manager_.GetRoom(room_id); };
	auto AddSession = [&](Session* session) { return session_manager_.AddSession(session); };
	auto RemoveSession = [&](uint32_t session_id) { session_manager_.RemoveSession(session_id); };
	auto GetSession = [&](uint32_t session_id) { return session_manager_.GetSession(session_id); };

	acceptor.Init(SavePacket, RemoveUser, GetRoom, AddSession, RemoveSession, GetSession);
}