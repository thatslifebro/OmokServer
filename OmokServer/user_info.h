#pragma once
#include <map>
#include <string>
#include <mutex>

// DB ¿ªÇÒ
class UserInfo
{
public:
	static std::map<std::string, std::string> user_login_info_map_; 
	static std::map<std::string, bool> user_login_status_map_;
	static std::mutex user_login_status_map_mutex_;

	void Init();

	bool Login(const std::string& id, const std::string& pw);

	void Logout(const std::string& id);
};
