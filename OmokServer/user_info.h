#pragma once
#include <map>
#include <string>

//			map<<id, pw>, is_login>
static std::map<std::pair<std::string, std::string>, bool> user_auth_map_ = {
	{std::make_pair("test1", "1"), false},
	{std::make_pair("test2", "2"), false},
	{std::make_pair("test3", "3"), false},
	{std::make_pair("test4", "4"), false},
	{std::make_pair("test5", "5"), false},
	{std::make_pair("test6", "6"), false},
	{std::make_pair("test7", "7"), false},
	{std::make_pair("test8", "8"), false},
	{std::make_pair("test9", "9"), false},
	{std::make_pair("test10", "10"), false}
};

//todo : 로그아웃 할 수 있도록 해야함