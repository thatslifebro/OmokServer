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
	uint16_t recursive_seconds_ = 0;
	bool is_recursive_ = false;

	std::function<void()> callback_;

	void SetTimer(int seconds, std::function<void()> callback)
	{
		std::lock_guard<std::mutex> lock(mutex);
		due_time_ = std::chrono::system_clock::now() + std::chrono::seconds(seconds);
		this->callback_ = callback;
		is_on_ = true;
	}

	void SetRecursiveTimer(int seconds, std::function<void()> callback)
	{
		std::lock_guard<std::mutex> lock(mutex);
		is_recursive_ = true;
		recursive_seconds_ = seconds;
		due_time_ = std::chrono::system_clock::now() + std::chrono::seconds(seconds);
		this->callback_ = callback;
		is_on_ = true;
	}

	void CancelTimer()
	{
		std::lock_guard<std::mutex> lock(mutex);
		is_on_ = false;
	}

	bool IsCallbackDone()
	{
		std::lock_guard<std::mutex> lock(mutex);
		auto temp = !is_on_;
		is_on_ = false;
		return temp;
	}
};