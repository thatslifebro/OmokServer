#include "game.h"

bool Game::SetStone(uint32_t x, uint32_t y, uint32_t session_id)
{
    if (x > 18 || y > 18)
    {
        return false;
    }

	if (!CheckTurn(session_id) || !IsGameStarted())
	{
		return false;
	}

	if (session_id == white_session_id_)
	{
		return SetStone(x, y, Status::WHITE);
	}
    else
    {
        return SetStone(x, y, Status::BLACK);
    }
}

void Game::Init(uint32_t white_session_id, uint32_t black_session_id)
{
    white_session_id_ = white_session_id;
    black_session_id_ = black_session_id;

    for (int i = 0; i < 19; i++)
    {
        for (int j = 0; j < 19; j++)
        {
			omok_board_.emplace_back(Status::EMPTY);
		}
	}
}

void Game::SetReady(uint32_t session_id)
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

bool Game::IsReady(uint32_t session_id)
{
    if (session_id == white_session_id_)
    {
		return white_ready_;
	}
    else if (session_id == black_session_id_)
    {
		return black_ready_;
	}
    else
    {
		return false;
	}
}

bool Game::IsGameStarted()
{
	return is_game_start_;
}

bool Game::IsGameEnd()
{
	return !is_game_start_;
}

bool Game::SetStone(uint32_t x, uint32_t y, Status color)
{
	if (omok_board_[Pos(x,y)] == Status::EMPTY)
	{
        omok_board_[Pos(x, y)] = color;
		if (color == Status::BLACK && CheckSamSam(x, y))
		{
            omok_board_[Pos(x, y)] = Status::EMPTY;
			return false;
		}
        else
        {
            ChangeTurn();
            return true;
        }
	}
    return false;
}

void Game::ChangeTurn()
{
	if (turn_ == Turn::WHITE_TURN)
	{
		turn_ = Turn::BLACK_TURN;
	}
	else
	{
		turn_ = Turn::WHITE_TURN;
	}
}

bool Game::CheckTurn(uint32_t session_id)
{
	if (session_id == white_session_id_ && turn_ == Turn::WHITE_TURN)
	{
		return true;
	}
	else if (session_id == black_session_id_ && turn_ == Turn::BLACK_TURN)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool Game::CheckOmok(int x, int y)
{
    if (가로확인(x, y) == 5)
    {
        is_game_start_ = false;
        return true;
    }

    else if (세로확인(x, y) == 5)
    {
        is_game_start_ = false;
        return true;
    }

    else if (사선확인(x, y) == 5)
    {
        is_game_start_ = false;
        return true;
    }

    else if (역사선확인(x, y) == 5)
    {
        is_game_start_ = false;
        return true;
    }

    return false;
}

#pragma region 오목확인

int Game::가로확인(int x, int y)      // ㅡ 확인
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

int Game::세로확인(int x, int y)      // | 확인
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

int Game::사선확인(int x, int y)      // / 확인
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

int Game::역사선확인(int x, int y)     // ＼ 확인
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

#pragma endregion

bool Game::CheckSamSam(int x, int y)
{
	int check = 0;

    check += 가로삼삼확인(x, y);
    check += 세로삼삼확인(x, y);
    check += 사선삼삼확인(x, y);
    check += 역사선삼삼확인(x, y);

    if (check >= 2)
    {
        return true;
    }

    return false;
}

#pragma region 삼삼확인

int Game::가로삼삼확인(int x, int y)    // 가로 (ㅡ) 확인
{
    int 돌3개확인 = 1;
    int 돌1 = 0;
    int 돌2 = 0;
    int i, j;

    for (i = 1; i <= 3; i++) // 돌을 둔 위치로부터 → 확인
    {
        if (x + i > 18)
            break;

        else if (omok_board_[Pos(x + i, y)] == omok_board_[Pos(x, y)])
            돌1++;

        else if (omok_board_[Pos(x + i, y)] != Status::EMPTY)
            break;
    }

    for (j = 1; j <= 3; j++) // 돌을 둔 위치로부터 ← 확인
    {
        if (x - j < 0)
            break;

        else if (omok_board_[Pos(x - j, y)] == omok_board_[Pos(x, y)])
            돌2++;

        else if (omok_board_[Pos(x - j, y)] != Status::EMPTY)
            break;
    }
    돌3개확인 += 돌1 + 돌2;

    if (돌3개확인 == 3 && x + 돌1 < 17 && x - 돌2 > 1)    //돌 개수가 3개면서 양쪽 벽에 붙어잇으면 안된다
    {
        if ((omok_board_[Pos(x + 돌1 + 2, y)] != Status::EMPTY && omok_board_[Pos(x + 돌1 + 1, y)] != Status::EMPTY) || (omok_board_[Pos(x - 돌2 - 2, y)] != Status::EMPTY && omok_board_[Pos(x - 돌2 - 1, y)] != Status::EMPTY))
        {
            return 0;
        }

        else
            return 1;
    }

    return 0;
}

int Game::세로삼삼확인(int x, int y)    // 세로 (|) 확인
{
    int 돌3개확인 = 1;
    int 돌1 = 0;
    int 돌2 = 0;
    int i, j;

    돌3개확인 = 1;

    for (i = 1; i <= 3; i++) // 돌을 둔 위치로부터 ↓ 확인
    {
        if (y + i > 18)
            break;

        else if (omok_board_[Pos(x, y + i)] == omok_board_[Pos(x, y)])
            돌1++;

        else if (omok_board_[Pos(x, y + i)] != Status::EMPTY)
            break;
    }

    for (j = 1; j <= 3; j++) // 돌을 둔 위치로부터 ↑ 확인
    {
        if (y - j < 0)
            break;

        else if (omok_board_[Pos(x, y - j)] == omok_board_[Pos(x, y)])
            돌2++;

        else if (omok_board_[Pos(x, y - j)] != Status::EMPTY)
            break;
    }

    돌3개확인 += 돌1 + 돌2;

    if (돌3개확인 == 3 && y + 돌1 < 17 && y - 돌2 > 1)    //돌 개수가 3개면서 양쪽 벽에 붙어잇으면 안된다
    {
        if ((omok_board_[Pos(x, y + 돌1 + 2)] != Status::EMPTY && omok_board_[Pos(x, y + 돌1 + 1)] != Status::EMPTY) || (omok_board_[Pos(x, y - 돌2 - 2)] != Status::EMPTY && omok_board_[Pos(x, y - 돌2 - 1)] != Status::EMPTY))
        {
            return 0;
        }

        else
            return 1;
    }

    return 0;
}

int Game::사선삼삼확인(int x, int y)    // 사선 (/) 확인
{
    int 돌3개확인 = 1;
    int 돌1 = 0;
    int 돌2 = 0;
    int i, j;

    돌3개확인 = 1;

    for (i = 1; i <= 3; i++) // 돌을 둔 위치로부터 ↗ 확인
    {
        if (x + i > 18 || y - i < 0)
            break;

        else if (omok_board_[Pos(x + i, y - i)] == omok_board_[Pos(x, y)])
            돌1++;

        else if (omok_board_[Pos(x + i, y - i)] != Status::EMPTY)
            break;
    }

    for (j = 1; j <= 3; j++) // 돌을 둔 위치로부터 ↙ 확인
    {
        if (x - j < 0 || y + j > 18)
            break;

        else if (omok_board_[Pos(x - j, y + j)] == omok_board_[Pos(x, y)])
            돌2++;

        else if (omok_board_[Pos(x - j, y + j)] != Status::EMPTY)
            break;
    }

    돌3개확인 += 돌1 + 돌2;

    if (돌3개확인 == 3 && x + 돌1 < 17 && y - 돌1 > 1 && x - 돌2 > 1 && y + 돌2 < 17)    //돌 개수가 3개면서 양쪽 벽에 붙어잇으면 안된다
    {
        if ((omok_board_[Pos(x + 돌1 + 2, y - 돌1 - 2)] != Status::EMPTY && omok_board_[Pos(x + 돌1 + 1, y - 돌1 - 1)] != Status::EMPTY) || (omok_board_[Pos(x - 돌2 -2, y + 돌2 + 2)] != Status::EMPTY && omok_board_[Pos(x - 돌2 - 1, y + 돌2 + 1)] != Status::EMPTY))
        {
            return 0;
        }

        else
            return 1;
    }

    return 0;
}

int Game::역사선삼삼확인(int x, int y)    // 역사선 (＼) 확인
{
    int 돌3개확인 = 1;
    int 돌1 = 0;
    int 돌2 = 0;
    int i, j;

    돌3개확인 = 1;

    for (i = 1; i <= 3; i++) // 돌을 둔 위치로부터 ↘ 확인
    {
        if (x + i > 18 || y + i > 18)
            break;

        else if (omok_board_[Pos(x + i, y + i)] == omok_board_[Pos(x, y)])
            돌1++;

        else if (omok_board_[Pos(x + i, y + i)] != Status::EMPTY)
            break;
    }

    for (j = 1; j <= 3; j++) // 돌을 둔 위치로부터 ↖ 확인
    {
        if (x - j < 0 || y - j < 0)
            break;

        else if (omok_board_[Pos(x - j, y - j)] == omok_board_[Pos(x, y)])
            돌2++;

        else if (omok_board_[Pos(x - j, y - j)] != Status::EMPTY)
            break;
    }

    돌3개확인 += 돌1 + 돌2;

    if (돌3개확인 == 3 && x + 돌1 < 17 && y + 돌1 < 17 && x - 돌2 > 1 && y - 돌2 > 1)    //돌 개수가 3개면서 양쪽 벽에 붙어잇으면 안된다
    {
        if ((omok_board_[Pos(x + 돌1 + 2, y + 돌1 + 2)] != Status::EMPTY && omok_board_[Pos(x + 돌1 + 1, y + 돌1 + 1)] != Status::EMPTY) || (omok_board_[Pos(x - 돌2 - 2, y - 돌2 - 2)] != Status::EMPTY && omok_board_[Pos(x - 돌2 - 1, y - 돌2 - 1)] != Status::EMPTY))
        {
            return 0;
        }

        else
            return 1;
    }

    return 0;
}

#pragma endregion

uint32_t Game::WinnerId()
{
	if (turn_ == Turn::WHITE_TURN)
	{
		return black_session_id_;
	}
	else
	{
		return white_session_id_;
	}
}

uint32_t Game::LoserId()
{
	if (turn_ == Turn::WHITE_TURN)
	{
		return white_session_id_;
	}
	else
	{
		return black_session_id_;
	}
}

int Game::Pos(int x, int y)
{
    return y * 19 + x;
}

void Game::EndGame()
{
    is_game_start_ = false;
    white_ready_ = false;
    black_ready_ = false;
    turn_ = Turn::BLACK_TURN;
    white_session_id_ = 0;
    black_session_id_ = 0;
    omok_board_.clear();
}