#include "db_processor.h"

std::queue<DBLoginReq> DBProcessor::login_queue_;
std::mutex DBProcessor::login_req_queue_mutex_;

void DBProcessor::Init()
{
	//db connect
}

bool DBProcessor::ProcessDB()
{
	if (login_queue_.empty())
	{
		return false;
	}
	
	ProcessLoginQueue();

	return true;
}

void DBProcessor::ProcessLoginQueue()
{
	std::lock_guard<std::mutex> lock(login_req_queue_mutex_);
	auto login_req = login_queue_.front();
	login_queue_.pop();

	auto user_id = login_req.user_id_;
	auto user_pw = login_req.user_pw_;

	int result = -1;

	auto iter = user_auth_map_.find(user_id);
	if (iter != user_auth_map_.end())
	{
		if (iter->second != user_pw)
		{
			result = 0;
		}
	}

	packet_sender_.ResLogin(login_req.session_, result);
}

void DBProcessor::AddLoginRequest(Session* session, const std::string& user_id, const std::string& user_pw)
{
	std::lock_guard<std::mutex> lock(login_req_queue_mutex_);
	login_queue_.push(DBLoginReq(session, user_id, user_pw));
}