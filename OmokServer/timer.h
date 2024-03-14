#pragma once
#include <chrono>
#include <functional>
#include <mutex>

class Timer
{
public:
	std::mutex mutex;
	bool is_on_ = false;
	std::chrono::system_clock::time_point due_time_;
	uint16_t repeated_seconds_ = 0;
	bool is_repeated_ = false;

	uint32_t num_ = 0;

	std::function<void()> callback_;

	void SetTimer(int seconds, std::function<void()> callback)
	{
		std::lock_guard<std::mutex> lock(mutex);
		due_time_ = std::chrono::system_clock::now() + std::chrono::seconds(seconds);
		this->callback_ = callback;
		is_on_ = true;
	}

	void SetRepeatedTimer(int seconds, std::function<void()> callback)
	{
		std::lock_guard<std::mutex> lock(mutex);
		is_repeated_ = true;
		repeated_seconds_ = seconds;
		due_time_ = std::chrono::system_clock::now() + std::chrono::seconds(seconds);
		this->callback_ = callback;
		is_on_ = true;
	}

	void CancelTimer()
	{
		std::lock_guard<std::mutex> lock(mutex);
		is_on_ = false;
	}

	void ContinueTimer()
	{
		std::lock_guard<std::mutex> lock(mutex);
		is_on_ = true;
	}

	bool IsCallbackDone()
	{
		std::lock_guard<std::mutex> lock(mutex);
		auto temp = !is_on_;
		is_on_ = false;
		return temp;
	}
};