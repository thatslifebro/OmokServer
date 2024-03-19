#include "omok_server.h"

void OmokServer::Init(flags::args args)
{
	auto [server_port, max_room_num] = ParseConfig(args);

	Poco::Net::ServerSocket server_socket(server_port);
	server_socket_ = server_socket;

	room_manager_.Init(max_room_num,
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

	packet_processor_.InitPacketQueueFunctions(
		[&]() { return packet_queue_.PopAndGetPacket(); },
		[&](const Packet& packet) {	db_packet_queue_.PushPacket(packet); });

	packet_processor_.InitRoomManagerFunctions(
		[&](uint32_t session_id, uint16_t room_id) { room_manager_.AddUser(session_id, room_id); },
		[&](uint32_t session_id, uint16_t room_id) { room_manager_.RemoveUser(session_id, room_id); },
		[&](uint16_t room_id) { return room_manager_.GetRoom(room_id); },
		[&]() { return room_manager_.GetAllRooms(); });

	packet_processor_.InitSessionManagerFunctions(
		[&](uint32_t session_id) { return session_manager_.GetSession(session_id); });

	packet_processor_.Init();

	db_processor_.InitDBPacketQueueFunctions( [&]() -> const Packet& { return db_packet_queue_.PopAndGetPacket(); });

	db_processor_.InitSessionManagerFunctions( [&](uint32_t session_id) { return session_manager_.GetSession(session_id); });

	db_processor_.Init();
}

void OmokServer::Start()
{
	std::jthread packet_processor_thread(&OmokServer::PacketProcessorStart, this);
	std::jthread db_processor_thread(&OmokServer::DBProcessorStart, this);

	Poco::Net::SocketReactor reactor;
	Poco::Net::ParallelSocketAcceptor<Session, Poco::Net::SocketReactor> acceptor(server_socket_, reactor);

	acceptor.Init([&](
		std::shared_ptr<char[]> buffer, uint32_t length, uint32_t session_id) { packet_queue_.Save(buffer, length, session_id); },
		[&](uint32_t session_id, uint16_t room_id) { room_manager_.RemoveUser(session_id, room_id);	},
		[&](uint16_t room_id) {	return room_manager_.GetRoom(room_id); },
		[&](Session* session) { return session_manager_.AddSession(session); },
		[&](uint32_t session_id) { session_manager_.RemoveSession(session_id); },
		[&](uint32_t session_id) { return session_manager_.GetSession(session_id); });

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

std::tuple<uint16_t, uint16_t> OmokServer::ParseConfig(flags::args args)
{
	const auto port = args.get<uint16_t>("port");

	uint16_t server_port;
	uint16_t max_room_num;

	if (!port)
	{
		server_port = SERVER_PORT;
	}
	else
	{
		server_port = *port;
	}

	const auto room_num = args.get<uint16_t>("room_num");
	if (!room_num)
	{
		max_room_num = MAX_ROOM_NUM;
	}
	else
	{
		max_room_num = *room_num;
	}

	return std::make_tuple(server_port, max_room_num);
}