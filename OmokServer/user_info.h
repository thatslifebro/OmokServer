#pragma once
#include <map>
#include <string>
#include <mutex>

class UserInfo
{
public:
	static std::map<std::string, std::string> user_login_info_map_; 
	static std::map<std::string, bool> user_login_status_map_;
	static std::mutex user_login_status_map_mutex_;

	void Init()
	{
		user_login_info_map_["user1"] = "user1";
		user_login_info_map_["user2"] = "user2";
		user_login_info_map_["user3"] = "user3";
		user_login_info_map_["user4"] = "user4";
		user_login_info_map_["user5"] = "user5";

		user_login_status_map_["user1"] = false;
		user_login_status_map_["user2"] = false;
		user_login_status_map_["user3"] = false;
		user_login_status_map_["user4"] = false;
		user_login_status_map_["user5"] = false;
	}

	bool Login(std::string id, std::string pw)
	{
		std::lock_guard<std::mutex> lock(user_login_status_map_mutex_);
		if (user_login_info_map_[id] == pw && user_login_status_map_[id] == false)
		{
			user_login_status_map_[id] = true;
			return true;
		}
		return false;
	}

	void Logout(std::string id)
	{
		std::lock_guard<std::mutex> lock(user_login_status_map_mutex_);
		user_login_status_map_[id] = false;
	}
};
