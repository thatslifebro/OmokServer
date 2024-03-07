#include "room.h"

std::vector<uint32_t> Room::GetSessionList() const
{
	std::vector<uint32_t> session_list;
	for (auto session_id : session_ids_)
	{
		session_list.push_back(session_id);
	}
	return session_list;
}