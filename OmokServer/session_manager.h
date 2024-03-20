#pragma once
#include <unordered_map>
#include <mutex>

#include "session.h"

class SessionManager
{
public:
	int AddSession(Session* session);

	void RemoveSession(uint32_t session_id);

	Session* GetSession(uint32_t session_id);

private:
	std::unordered_map<uint32_t, Session*> session_map_;

	int FindEmptySessionId();

	bool IsSessionExist(uint32_t session_id);
};