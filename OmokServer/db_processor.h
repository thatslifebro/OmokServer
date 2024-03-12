#pragma once
#include <unordered_map>
#include <string>
#include <queue>
#include <mutex>
#include <functional>

#include "session_manager.h"
#include "packet_sender.h"

struct DBLoginReq
{
	uint32_t session_id_;
	std::string user_id_;
	std::string user_pw_;
};

class DBProcessor
{
public:
	UserInfo user_info_;

	void Init(); // DB 연결

	void AddLoginReq(uint32_t session_id, const std::string& user_name, const std::string& password);

	bool ProcessDB(); // DB쓰레드에서 실행될 함수

private:
	static std::queue<DBLoginReq> login_queue_;
	static std::mutex login_req_queue_mutex_;

	SessionManager session_manager_;

	void ProcessLoginQueue();

	std::function<void(Session*, uint32_t)> SendResLogin;
};