#pragma once
#include <map>

#include "session.h"

struct Pos
{
	Pos(uint16_t x, uint16_t y) : x_(x), y_(y)
	{
	}
	uint16_t x_;
	uint16_t y_;
	
	bool operator<(const Pos& pos) const
	{
		return x_ < pos.x_ || (x_ == pos.x_ && y_ < pos.y_);
	}
};

enum Turn
{
	BLACK_TURN,
	WHITE_TURN	
};

enum status
{
	EMPTY,
	WHITE,
	BLACK
};

class GameRoom
{
public:
	GameRoom() = default;
	GameRoom(uint16_t white_session_id, uint16_t black_session_id) : white_session_id_(white_session_id), black_session_id_(black_session_id), turn_(BLACK_TURN), white_ready_(false), black_ready_(false), is_game_start_(false)
	{}

	void Init();

	bool SetStone(uint16_t x, uint16_t y, uint16_t session_id);
	
	void SetReady(uint16_t session_id);

	bool IsGameStart();

	bool IsGameEnd();


private:
	uint16_t white_session_id_;
	uint16_t black_session_id_;
	bool white_ready_;
	bool black_ready_;
	bool is_game_start_;
	Turn turn_;	
	std::map <Pos, status> omok_board_;

	bool SetStone(uint16_t x, uint16_t y, status color);

	void ChangeTurn();

	bool CheckTurn(uint16_t session_id);

	bool CheckRule();
};
