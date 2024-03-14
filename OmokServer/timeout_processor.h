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
		// room에 있는 타이머들을 저장
		RoomManager room_manager;
		auto rooms = room_manager.GetAllRooms();
		for (auto room : rooms)
		{
			timer_vec_.push_back(room->timer_);
		}
	}

	void ProcessTimeout()
	{
		//타이머를 하나씩 확인하며 on 인지, 시간이 지났는지 확인 후 콜백 호출
		for (auto& timer : timer_vec_)
		{
			std::lock_guard<std::mutex> lock(timer->mutex);
			if(timer->is_on_ && timer->due_time_ <= std::chrono::system_clock::now())
			{
				timer->is_on_ = false;
				timer->callback_();
				//반복 타이머인 경우 다시 설정(돌 두기 이후 상대에게도 타이머가 걸려야하기 때문)
				if (timer->is_repeated_)
				{
					timer->due_time_ = std::chrono::system_clock::now() + std::chrono::seconds(timer->duration_);
					timer->is_on_ = true;
				}
			}
		}
	}
};