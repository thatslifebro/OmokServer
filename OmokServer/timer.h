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

	void SetTimer(uint32_t time_count, uint32_t duration, std::function<void()> callback)
	{
		time_count_ = time_count + duration;
		this->callback_ = callback;
		is_on_ = true;
		is_repeated_ = false;
	}

	void SetRepeatedTimer(uint32_t time_count, uint32_t duration, std::function<void()> callback)
	{
		time_count_ = time_count + duration;
		duration_ = duration;
		this->callback_ = callback;
		is_on_ = true;
		is_repeated_ = true;
	}

	// 타이머를 같은 방식으로 한번 더 설정
	void SetSameWithPreviousTimer(uint32_t time_count)
	{
		if (callback_ == nullptr || duration_ <= 0)
		{
			return;
		}
		time_count_ = time_count + duration_;
		is_on_ = true;
	}

	void CancelTimer()
	{
		is_on_ = false;
	}

	void Check(uint32_t time_count)
	{
		if (is_on_ && time_count_ <= time_count)
		{
			callback_();
			is_on_ = false;
			if (is_repeated_)
			{
				is_on_ = true;
				time_count_ = time_count + duration_;
			}
		}
	}
};