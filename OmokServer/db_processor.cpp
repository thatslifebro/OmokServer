#include "db_processor.h"

std::queue<DBLoginReq> DBProcessor::login_queue_;
std::mutex DBProcessor::login_req_queue_mutex_;

void DBProcessor::Init()
{
	// 이렇게 쓰는 것이 좋은지, 그냥 PacketSender 객체를 멤버 변수로 가지고 packet_sende_.ResLogin(session, result); 이런식으로 쓰는 것이 좋은지 모르겠다. 
	SendResLogin = [](Session* session, uint32_t result)
		{
			PacketSender packet_sender;
			packet_sender.ResLogin(session, result);
		};
	//db가 실제 있다면 connect 한다.
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
	auto session = session_manager_.GetSession(login_req.session_id_);

	if (session == nullptr)
	{
		return;
	}

	uint32_t result = -1;

	//DB에 로그인 처리
	if (user_info_.Login(user_id, user_pw) == true)
	{
		result = 0;
		session->user_id_ = user_id;
		session->is_logged_in_ = true;
	}

	SendResLogin(session, result);
}

void DBProcessor::AddLoginReq(uint32_t session_id, const std::string& user_id, const std::string& user_pw)
{
	std::lock_guard<std::mutex> lock(login_req_queue_mutex_);
	login_queue_.push(DBLoginReq(session_id, user_id, user_pw));
}
