#include "session_manager.h"

int SessionManager::AddSession(Session* session)
{
	auto session_id = FindEmptySessionId();

	session_map_.insert({ session_id, session });

	return session_id;
}

void SessionManager::RemoveSession(uint32_t session_id)
{
	auto session = GetSession(session_id);
	delete session;

	session_map_.erase(session_id);
}

Session* SessionManager::GetSession(uint32_t session_id)
{
	if (IsSessionExist(session_id) == false)
	{
		return nullptr;
	};

	return session_map_[session_id];
}

bool SessionManager::IsSessionExist(uint32_t session_id)
{
	return session_map_.find(session_id) != session_map_.end();
}

int SessionManager::FindEmptySessionId()
{
	uint32_t session_id = 1;
	while (session_map_.find(session_id) != session_map_.end())
	{
		session_id++;
	}
	return session_id;
}