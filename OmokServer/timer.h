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
	uint16_t duration_ = 0;
	bool is_repeated_ = false;

	std::function<void()> callback_;

	void SetTimer(int seconds, std::function<void()> callback)
	{
		std::lock_guard<std::mutex> lock(mutex);
		due_time_ = std::chrono::system_clock::now() + std::chrono::seconds(seconds);
		duration_ = seconds;
		this->callback_ = callback;
		is_on_ = true;

		is_repeated_ = false;
	}

	void SetRepeatedTimer(int seconds, std::function<void()> callback)
	{
		std::lock_guard<std::mutex> lock(mutex);
		due_time_ = std::chrono::system_clock::now() + std::chrono::seconds(seconds);
		duration_ = seconds;
		this->callback_ = callback;
		is_on_ = true;

		is_repeated_ = true;
	}

	// 타이머를 같은 방식으로 한번 더 설정
	void SetSameWithPreviousTimer()
	{
		if (callback_ == nullptr || duration_ <= 0)
		{
			return;
		}
		std::lock_guard<std::mutex> lock(mutex);
		due_time_ = std::chrono::system_clock::now() + std::chrono::seconds(duration_);
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