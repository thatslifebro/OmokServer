#include "timer.h"

void Timer::SetTimer(uint32_t time_count, uint32_t duration, std::function<void()> callback)
{
	time_count_ = time_count + duration;
	this->callback_ = callback;
	is_on_ = true;
	is_repeated_ = false;
}

void Timer::SetRepeatedTimer(uint32_t time_count, uint32_t duration, std::function<void()> callback)
{
	time_count_ = time_count + duration;
	duration_ = duration;
	this->callback_ = callback;
	is_on_ = true;
	is_repeated_ = true;
}

void Timer::SetSameWithPreviousTimer(uint32_t time_count)
{
	if (callback_ == nullptr || duration_ <= 0)
	{
		return;
	}
	time_count_ = time_count + duration_;
	is_on_ = true;
}

void Timer::Check(uint32_t time_count)
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