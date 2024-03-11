#include "user_info.h"

std::map<std::string, std::string> UserInfo::user_login_info_map_;
std::map<std::string, bool> UserInfo::user_login_status_map_;
std::mutex UserInfo::user_login_status_map_mutex_;