#pragma once
#include <chrono>
#include <functional>
#include <mutex>

class Timer
{
public:
	bool is_on_ = false;
	uint32_t time_count_ = 0;
	uint16_t duration_ = 0;
	bool is_repeated_ = false;

	std::function<void()> callback_;

	void SetTimer(uint32_t time_count, uint32_t duration, std::function<void()> callback);

	void SetRepeatedTimer(uint32_t time_count, uint32_t duration, std::function<void()> callback);

	// 타이머를 같은 방식으로 한번 더 설정
	void SetSameWithPreviousTimer(uint32_t time_count);

	void CancelTimer() { is_on_ = false; }

	void Check(uint32_t time_count);
};