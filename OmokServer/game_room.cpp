#include "game_room.h"

void GameRoom::Init()
{
	for (int i = 0; i < 19; ++i)
	{
		for (int j = 0; j < 19; ++j)
		{
			Pos pos(i, j);
			omok_board_[pos] = EMPTY;
		}
	}
}

bool GameRoom::SetStone(uint16_t x, uint16_t y, uint16_t session_id)
{
	if (!CheckTurn(session_id) || !IsGameStart())
	{
		return false;
	}

	if (session_id == white_session_id_)
	{
		return SetStone(x, y, WHITE);
	}
	else if (session_id == black_session_id_)
	{
		return SetStone(x, y, BLACK);
	}
	else {
		return false;
	}
}

void GameRoom::SetReady(uint16_t session_id)
{
	if (session_id == white_session_id_)
	{
		white_ready_ = true;
	}
	else if (session_id == black_session_id_)
	{
		black_ready_ = true;
	}
}

bool GameRoom::IsGameStart()
{
	if (white_ready_ && black_ready_)
	{
		is_game_start_ = true;
	}
	return is_game_start_;
}

bool GameRoom::IsGameEnd()
{
	//todo:: check game end
	return false;
}

bool GameRoom::SetStone(uint16_t x, uint16_t y, status color)
{
	Pos pos(x, y);
	if (omok_board_[pos] == EMPTY)
	{
		omok_board_[pos] = color;
		if (CheckRule())
		{
			ChangeTurn();
			return true;
		}
		else
		{
			omok_board_[pos] = EMPTY;
			return false;
		}
	}
	return false;
}

void GameRoom::ChangeTurn()
{
	if (turn_ == WHITE_TURN)
	{
		turn_ = BLACK_TURN;
	}
	else
	{
		turn_ = WHITE_TURN;
	}
}

bool GameRoom::CheckTurn(uint16_t session_id)
{
	if (session_id == white_session_id_ && turn_ == WHITE_TURN)
	{
		return true;
	}
	else if (session_id == black_session_id_ && turn_ == BLACK_TURN)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool GameRoom::CheckRule()
{
	// todo: check rule
	return true;
}