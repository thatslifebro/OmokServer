#pragma once
#include <chrono>
#include <functional>
#include <mutex>

class Timer
{
public:
	std::function<void()> callback_;

	void SetTimer(uint32_t time_count, uint32_t duration, std::function<void()> callback);

	void SetRepeatedTimer(uint32_t time_count, uint32_t duration, std::function<void()> callback);

	void SetSameWithPreviousTimer(uint32_t time_count);

	void CancelTimer() { is_on_ = false; }

	void Check(uint32_t time_count);

private:
	bool is_on_ = false;
	uint32_t time_count_ = 0;
	uint16_t duration_ = 0;
	bool is_repeated_ = false;
};