#pragma once
#include <vector>

#include "timer.h"
#include "room_manager.h"

class TimeoutProcessor
{
public:
	std::vector<Timer*> timer_vec_;

	void Init()
	{
		RoomManager room_manager;
		auto rooms = room_manager.GetAllRooms();
		for (auto room : rooms)
		{
			timer_vec_.push_back(room->timer_);
		}
	}

	void ProcessTimeout()
	{
		for (auto& timer : timer_vec_)
		{
			std::lock_guard<std::mutex> lock(timer->mutex);
			if(timer->due_time_ <= std::chrono::system_clock::now() && timer->is_on_)
			{
				timer->is_on_ = false;
				timer->callback_();
				if (timer->is_repeated_)
				{
					timer->due_time_ = std::chrono::system_clock::now() + std::chrono::seconds(timer->duration_);
					timer->is_on_ = true;
				}
			}
		}
	}
};