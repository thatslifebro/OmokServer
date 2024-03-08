#pragma once
#include <unordered_map>
#include <string>

static std::unordered_map<std::string, std::string> user_auth_map_ = {
	{"test1", "1"},
	{"test2", "2"},
	{"test3", "3"},
	{"test4", "4"},
	{"test5", "5"},
	{"test6", "6"},
	{"test7", "7"},
	{"test8", "8"},
	{"test9", "9"},
	{"test10", "10"}
};

//todo : 이미 로그인된 사용자는 로그인을 할 수 없도록 처리해야함