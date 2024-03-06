#include "session_manager.h"

std::unordered_map<uint32_t, Session*> SessionManager::session_map_;
std::mutex SessionManager::session_map_mutex_;

int SessionManager::AddSession(Session* session)
{
	auto session_id = FindEmptySessionId();

	std::lock_guard<std::mutex> lock(session_map_mutex_);
	session_map_.insert({ session_id, session });

	return session_id;
}

void SessionManager::RemoveSession(uint32_t session_id)
{
	std::lock_guard<std::mutex> lock(session_map_mutex_);
	session_map_.erase(session_id);
}

Session* SessionManager::GetSession(uint32_t session_id)
{
	return session_map_[session_id];
}

bool SessionManager::IsSessionExist(uint32_t session_id)
{
	return session_map_.find(session_id) != session_map_.end();
}

int SessionManager::FindEmptySessionId()
{
	int session_id = 1;
	while (session_map_.find(session_id) != session_map_.end())
	{
		session_id++;
	}
	return session_id;
}