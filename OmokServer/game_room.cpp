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

bool GameRoom::SetStone(uint32_t x, uint32_t y, uint32_t session_id)
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

void GameRoom::SetReady(uint32_t session_id)
{
	if (session_id == white_session_id_)
	{
		white_ready_ = true;
	}
	else if (session_id == black_session_id_)
	{
		black_ready_ = true;
	}

	if (white_ready_ && black_ready_)
	{
		is_game_start_ = true;
	}
}

bool GameRoom::IsGameStart()
{
	return is_game_start_;
}

bool GameRoom::IsGameEnd()
{
	return !is_game_start_;
}

int GameRoom::SetStone(uint32_t x, uint32_t y, status color)
{
	Pos pos(x, y);
	if (omok_board_[pos] == EMPTY)
	{
        omok_board_[pos] = color;
		if (color == BLACK && CheckSamSam(pos))
		{
            omok_board_[pos] = EMPTY;
			return 0;
		}
        else
        {
            if (CheckOmok(pos))
            {
                return 2;
            }
            ChangeTurn();
            return 1;
        }
	}
    return 0;
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

bool GameRoom::CheckTurn(uint32_t session_id)
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

bool GameRoom::CheckOmok(Pos pos)
{
    if (가로확인(pos.x_, pos.y_) == 5)        // 같은 돌 개수가 5개면 (6목이상이면 게임 계속) 
    {
        is_game_start_ = false;
        return true;
    }

    else if (세로확인(pos.x_, pos.y_) == 5)
    {
        is_game_start_ = false;
        return true;
    }

    else if (사선확인(pos.x_, pos.y_) == 5)
    {
        is_game_start_ = false;
        return true;
    }

    else if (역사선확인(pos.x_, pos.y_) == 5)
    {
        is_game_start_ = false;
        return true;
    }

    return false;
}

int GameRoom::가로확인(int x, int y)      // ㅡ 확인
{
    int 같은돌개수 = 1;

    for (int i = 1; i <= 5; i++)
    {
        if (x + i <= 18 && omok_board_[Pos(x + i, y)] == omok_board_[Pos(x, y)])
            같은돌개수++;

        else
            break;
    }

    for (int i = 1; i <= 5; i++)
    {
        if (x - i >= 0 && omok_board_[Pos(x - i, y)] == omok_board_[Pos(x, y)])
            같은돌개수++;

        else
            break;
    }

    return 같은돌개수;
}

int GameRoom::세로확인(int x, int y)      // | 확인
{
    int 같은돌개수 = 1;

    for (int i = 1; i <= 5; i++)
    {
        if (y + i <= 18 && omok_board_[Pos(x, y + i)] == omok_board_[Pos(x, y)])
            같은돌개수++;

        else
            break;
    }

    for (int i = 1; i <= 5; i++)
    {
        if (y - i >= 0 && omok_board_[Pos(x, y - i)] == omok_board_[Pos(x, y)])
            같은돌개수++;

        else
            break;
    }

    return 같은돌개수;
}

int GameRoom::사선확인(int x, int y)      // / 확인
{
    int 같은돌개수 = 1;

    for (int i = 1; i <= 5; i++)
    {
        if (x + i <= 18 && y - i >= 0 && omok_board_[Pos(x + i, y - i)] == omok_board_[Pos(x, y)])
            같은돌개수++;

        else
            break;
    }

    for (int i = 1; i <= 5; i++)
    {
        if (x - i >= 0 && y + i <= 18 && omok_board_[Pos(x - i, y + i)] == omok_board_[Pos(x, y)])
            같은돌개수++;

        else
            break;
    }

    return 같은돌개수;
}

int GameRoom::역사선확인(int x, int y)     // ＼ 확인
{
    int 같은돌개수 = 1;

    for (int i = 1; i <= 5; i++)
    {
        if (x + i <= 18 && y + i <= 18 && omok_board_[Pos(x + i, y + i)] == omok_board_[Pos(x, y)])
            같은돌개수++;

        else
            break;
    }

    for (int i = 1; i <= 5; i++)
    {
        if (x - i >= 0 && y - i >= 0 && omok_board_[Pos(x - i, y - i)] == omok_board_[Pos(x, y)])
            같은돌개수++;

        else
            break;
    }

    return 같은돌개수;
}

bool GameRoom::CheckSamSam(Pos pos)
{
	// todo: check rule
	int check = 0;

    check += 가로삼삼확인(pos.x_, pos.y_);
    check += 세로삼삼확인(pos.x_, pos.y_);
    check += 사선삼삼확인(pos.x_, pos.y_);
    check += 역사선삼삼확인(pos.x_, pos.y_);

    if (check >= 2)
        return true;

    else
        return false;

	return true;
}

int GameRoom::가로삼삼확인(int x, int y)    // 가로 (ㅡ) 확인
{
    int 돌3개확인 = 1;
    int i, j;

    for (i = 1; i <= 3; i++) // 돌을 둔 위치로부터 → 확인
    {
        if (x + i > 18)
            break;

        else if (omok_board_[Pos(x + i, y)] == omok_board_[Pos(x, y)])
            돌3개확인++;

        else if (omok_board_[Pos(x + i, y)] != EMPTY)
            break;
    }

    for (j = 1; j <= 3; j++) // 돌을 둔 위치로부터 ← 확인
    {
        if (x - j < 0)
            break;

        else if (omok_board_[Pos(x - j, y)] == omok_board_[Pos(x, y)])
            돌3개확인++;

        else if (omok_board_[Pos(x - j, y)] != EMPTY)
            break;
    }

    if (돌3개확인 == 3 && x + i < 18 && x - j > 0)    //돌 개수가 3개면서 양쪽 벽에 붙어잇으면 안된다
    {
        if ((omok_board_[Pos(x + i, y)] != EMPTY && omok_board_[Pos(x + i - 1, y)] != EMPTY) || (omok_board_[Pos(x - j, y)] != EMPTY && omok_board_[Pos(x - j + 1, y)] != EMPTY))
        {
            return 0;
        }

        else
            return 1;
    }

    return 0;
}

int GameRoom::세로삼삼확인(int x, int y)    // 세로 (|) 확인
{
    int 돌3개확인 = 1;
    int i, j;

    돌3개확인 = 1;

    for (i = 1; i <= 3; i++) // 돌을 둔 위치로부터 ↓ 확인
    {
        if (y + i > 18)
            break;

        else if (omok_board_[Pos(x, y + i)] == omok_board_[Pos(x, y)])
            돌3개확인++;

        else if (omok_board_[Pos(x, y + i)] != EMPTY)
            break;
    }

    for (j = 1; j <= 3; j++) // 돌을 둔 위치로부터 ↑ 확인
    {
        if (y - j < 0)
            break;

        else if (omok_board_[Pos(x, y - j)] == omok_board_[Pos(x, y)])
            돌3개확인++;

        else if (omok_board_[Pos(x, y - j)] != EMPTY)
            break;
    }

    if (돌3개확인 == 3 && y + i < 18 && y - j > 0)    //돌 개수가 3개면서 양쪽 벽에 붙어잇으면 안된다
    {
        if ((omok_board_[Pos(x, y + i)] != EMPTY && omok_board_[Pos(x, y + i - 1)] != EMPTY) || (omok_board_[Pos(x, y - j)] != EMPTY && omok_board_[Pos(x, y - j + 1)] != EMPTY))
        {
            return 0;
        }

        else
            return 1;
    }

    return 0;
}

int GameRoom::사선삼삼확인(int x, int y)    // 사선 (/) 확인
{
    int 돌3개확인 = 1;
    int i, j;

    돌3개확인 = 1;

    for (i = 1; i <= 3; i++) // 돌을 둔 위치로부터 ↗ 확인
    {
        if (x + i > 18 || y - i < 0)
            break;

        else if (omok_board_[Pos(x + i, y - i)] == omok_board_[Pos(x, y)])
            돌3개확인++;

        else if (omok_board_[Pos(x + i, y - i)] != EMPTY)
            break;
    }

    for (j = 1; j <= 3; j++) // 돌을 둔 위치로부터 ↙ 확인
    {
        if (x - j < 0 || y + j > 18)
            break;

        else if (omok_board_[Pos(x - j, y + j)] == omok_board_[Pos(x, y)])
            돌3개확인++;

        else if (omok_board_[Pos(x - j, y + j)] != EMPTY)
            break;
    }

    if (돌3개확인 == 3 && x + i < 18 && y - i > 0 && x - j > 0 && y + j < 18)    //돌 개수가 3개면서 양쪽 벽에 붙어잇으면 안된다
    {
        if ((omok_board_[Pos(x + i, y - i)] != EMPTY && omok_board_[Pos(x + i - 1, y - i + 1)] != EMPTY) || (omok_board_[Pos(x - j, y + j)] != EMPTY && omok_board_[Pos(x - j + 1, y + j - 1)] != EMPTY))
        {
            return 0;
        }

        else
            return 1;
    }

    return 0;
}

int GameRoom::역사선삼삼확인(int x, int y)    // 역사선 (＼) 확인
{
    int 돌3개확인 = 1;
    int i, j;

    돌3개확인 = 1;

    for (i = 1; i <= 3; i++) // 돌을 둔 위치로부터 ↘ 확인
    {
        if (x + i > 18 || y + i > 18)
            break;

        else if (omok_board_[Pos(x + i, y + i)] == omok_board_[Pos(x, y)])
            돌3개확인++;

        else if (omok_board_[Pos(x + i, y + i)] != EMPTY)
            break;
    }

    for (j = 1; j <= 3; j++) // 돌을 둔 위치로부터 ↖ 확인
    {
        if (x - j < 0 || y - j < 0)
            break;

        else if (omok_board_[Pos(x - j, y - j)] == omok_board_[Pos(x, y)])
            돌3개확인++;

        else if (omok_board_[Pos(x - j, y - j)] != EMPTY)
            break;
    }

    if (돌3개확인 == 3 && x + i < 18 && y + i < 18 && x - j > 0 && y - j > 0)    //돌 개수가 3개면서 양쪽 벽에 붙어잇으면 안된다
    {
        if ((omok_board_[Pos(x + i, y + i)] != EMPTY && omok_board_[Pos(x + i - 1, y + i - 1)] != EMPTY) || (omok_board_[Pos(x - j, y - j)] != EMPTY && omok_board_[Pos(x - j + 1, y - j + 1)] != EMPTY))
        {
            return 0;
        }

        else
            return 1;
    }

    return 0;
}

uint32_t GameRoom::GetOpponentId(uint32_t session_id)
{
	if (session_id == white_session_id_)
	{
		return black_session_id_;
	}
	else if (session_id == black_session_id_)
	{
		return white_session_id_;
	}
	else
	{
		return 0;
	}
}

uint32_t GameRoom::GetWhiteSessionId()
{
	return white_session_id_;
}

uint32_t GameRoom::GetBlackSessionId()
{
	return black_session_id_;
}

uint32_t GameRoom::WinnerId()
{
    if (is_game_start_)
    {
        return 0;
    }
    else
	{
		if (turn_ == WHITE_TURN)
		{
			return white_session_id_;
		}
		else
		{
			return black_session_id_;
		}
	}
}

uint32_t GameRoom::LoserId()
{
	if (is_game_start_)
	{
		return 0;
	}
	else
	{
		if (turn_ == WHITE_TURN)
		{
			return black_session_id_;
		}
		else
		{
			return white_session_id_;
		}
	}
}