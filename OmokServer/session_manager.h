#pragma once
#include <unordered_map>
#include <mutex>

class Session;

class SessionManager
{
public:
	int AddSession(Session* session);

	void RemoveSession(uint32_t session_id);

	Session* GetSession(uint32_t session_id);

	bool IsSessionExist(uint32_t session_id);

private:
	static std::unordered_map<uint32_t, Session*> session_map_;
	static std::mutex session_map_mutex_;

	int FindEmptySessionId();
};