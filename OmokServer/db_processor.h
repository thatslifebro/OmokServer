#pragma once
#include <unordered_map>
#include <string>
#include <queue>
#include <mutex>

#include "user_info.h"
#include "session.h"
#include "packet_sender.h"

struct DBLoginReq
{
	Session* session_;
	std::string user_id_;
	std::string user_pw_;
};

class DBProcessor
{
public:
	PacketSender packet_sender_;

	void Init();

	void AddLoginRequest(Session* session, const std::string& user_name, const std::string& password);

	bool ProcessDB();

private:
	static std::queue<DBLoginReq> login_queue_;
	static std::mutex login_req_queue_mutex_;


	void ProcessLoginQueue();

};