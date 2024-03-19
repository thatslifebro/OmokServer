#pragma once
#include <unordered_map>
#include <mutex>

class Session;

class SessionManager // todo:: 이것도 가능할까 static 없애기
{
public:
	int AddSession(Session* session);

	void RemoveSession(uint32_t session_id);

	Session* GetSession(uint32_t session_id);

private:
	static std::mutex session_map_mutex_;

	std::unordered_map<uint32_t, Session*> session_map_;

	int FindEmptySessionId();

	bool IsSessionExist(uint32_t session_id);
};