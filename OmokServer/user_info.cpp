#include "user_info.h"

std::map<std::string, std::string> UserInfo::user_login_info_map_;
std::map<std::string, bool> UserInfo::user_login_status_map_;
std::mutex UserInfo::user_login_status_map_mutex_;

void UserInfo::Init()
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

bool UserInfo::Login(const std::string& id, const std::string& pw)
{
	std::lock_guard<std::mutex> lock(user_login_status_map_mutex_);
	
	// 아이디 비밀번호 확인, 로그인 상태 확인
	if (user_login_info_map_[id] == pw && user_login_status_map_[id] == false)
	{
		user_login_status_map_[id] = true;
		return true;
	}
	return false;
}

void UserInfo::Logout(const std::string& id)
{
	std::lock_guard<std::mutex> lock(user_login_status_map_mutex_);
	user_login_status_map_[id] = false;
}