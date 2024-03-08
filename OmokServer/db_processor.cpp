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

	auto output = user_auth_map_
		| std::views::filter([&](const auto& pair) { return pair.first == std::make_pair(user_id, user_pw); })
		| std::views::filter([&](const auto& pair) { return pair.second == false; });

	if (!output.empty())
	{
		user_auth_map_[std::make_pair(user_id, user_pw)] = true;
		result = 0;
		login_req.session_->user_id_ = user_id;
		login_req.session_->is_logged_in_ = true;
	}

	packet_sender_.ResLogin(login_req.session_, result);
}

void DBProcessor::AddLoginRequest(Session* session, const std::string& user_id, const std::string& user_pw)
{
	std::lock_guard<std::mutex> lock(login_req_queue_mutex_);
	login_queue_.push(DBLoginReq(session, user_id, user_pw));
}