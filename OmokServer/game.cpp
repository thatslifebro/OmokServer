#include "game.h"

bool Game::CanSetStone(uint32_t x, uint32_t y, uint32_t session_id)
{
    if (x > 18 || y > 18)
    {
        return false;
    }

    if (session_id == white_session_id_)
    {
        return CanSetStone(x, y, Status::WHITE);
    }
    else
    {
        return CanSetStone(x, y, Status::BLACK);
    }
}

void Game::SetStone(uint32_t x, uint32_t y, uint32_t session_id)
{

	if (session_id == white_session_id_)
	{
		SetStone(x, y, Status::WHITE);
	}
    else
    {
        SetStone(x, y, Status::BLACK);
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
	else
	{
		black_ready_ = true;
	}
}

void Game::StartGameIfBothReady()
{
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
    else
    {
		return black_ready_;
	}
}

bool Game::CanSetStone(uint32_t x, uint32_t y, Status color)
{
    if (omok_board_[Pos(x, y)] == Status::EMPTY)
    {
        omok_board_[Pos(x, y)] = color;

        if (color == Status::BLACK && CheckSamSam(x, y))
        {
            omok_board_[Pos(x, y)] = Status::EMPTY;
            return false;
        }
        else
        {
            omok_board_[Pos(x, y)] = Status::EMPTY;
            return true;
        }
    }

    return false;
}

void Game::SetStone(uint32_t x, uint32_t y, Status color)
{
    omok_board_[Pos(x, y)] = color;

    ChangeTurn();
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
    if (CheckOmokRow(x, y) == 5)
    {
        is_game_start_ = false;
        return true;
    }
    else if (CheckOmokColumn(x, y) == 5)
    {
        is_game_start_ = false;
        return true;
    }
    else if (CheckOmokSlash(x, y) == 5)
    {
        is_game_start_ = false;
        return true;
    }
    else if (CheckOmokBackSlash(x, y) == 5)
    {
        is_game_start_ = false;
        return true;
    }

    return false;
}

#pragma region ����Ȯ��

int Game::CheckOmokRow(int x, int y)
{
    int mok = 1;

    for (int i = 1; i <= 5; i++)
    {
        if (x + i <= 18 && omok_board_[Pos(x + i, y)] == omok_board_[Pos(x, y)])
        {
            mok++;
        }
        else
        {
            break;
        }
    }

    for (int i = 1; i <= 5; i++)
    {
        if (x - i >= 0 && omok_board_[Pos(x - i, y)] == omok_board_[Pos(x, y)])
        {
            mok++;
        }
        else
        {
            break;
        }
    }

    return mok;
}

int Game::CheckOmokColumn(int x, int y)      // | Ȯ��
{
    int mok = 1;

    for (int i = 1; i <= 5; i++)
    {
        if (y + i <= 18 && omok_board_[Pos(x, y + i)] == omok_board_[Pos(x, y)])
        {
            mok++;
        }
        else
        {
            break;
        }
    }

    for (int i = 1; i <= 5; i++)
    {
        if (y - i >= 0 && omok_board_[Pos(x, y - i)] == omok_board_[Pos(x, y)])
        {
            mok++;
        }
        else
        {
            break;
        }
    }

    return mok;
}

int Game::CheckOmokSlash(int x, int y)      // / Ȯ��
{
    int mok = 1;

    for (int i = 1; i <= 5; i++)
    {
        if (x + i <= 18 && y - i >= 0 && omok_board_[Pos(x + i, y - i)] == omok_board_[Pos(x, y)])
        {
            mok++;
        }
        else
        {
            break;
        }
    }

    for (int i = 1; i <= 5; i++)
    {
        if (x - i >= 0 && y + i <= 18 && omok_board_[Pos(x - i, y + i)] == omok_board_[Pos(x, y)])
        {
            mok++;
        }
        else
        {
            break;
        }
    }

    return mok;
}

int Game::CheckOmokBackSlash(int x, int y)     // �� Ȯ��
{
    int mok = 1;

    for (int i = 1; i <= 5; i++)
    {
        if (x + i <= 18 && y + i <= 18 && omok_board_[Pos(x + i, y + i)] == omok_board_[Pos(x, y)])
        {
            mok++;
        }
        else
        {
            break;
        }
    }

    for (int i = 1; i <= 5; i++)
    {
        if (x - i >= 0 && y - i >= 0 && omok_board_[Pos(x - i, y - i)] == omok_board_[Pos(x, y)])
        {
            mok++;
        }
        else
        {
            break;
        }
    }

    return mok;
}

#pragma endregion

bool Game::CheckSamSam(int x, int y)
{
	int check = 0;

    check += CheckSamSamRow(x, y);
    check += CheckSamSamColumn(x, y);
    check += CheckSamSamSlash(x, y);
    check += CheckSamSamBackSlash(x, y);

    if (check >= 2)
    {
        return true;
    }

    return false;
}

#pragma region ���Ȯ��

int Game::CheckSamSamRow(int x, int y)    // ���� (��) Ȯ��
{
    int mok = 1;
    int right_mok = 0;
    int left_mok = 0;
    int i, j;

    for (i = 1; i <= 3; i++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (x + i > 18)
        {
            break;
        }
        else if (omok_board_[Pos(x + i, y)] == omok_board_[Pos(x, y)])
        {
            right_mok++;
        }
        else if (omok_board_[Pos(x + i, y)] != Status::EMPTY)
        {
            break;
        }
    }

    for (j = 1; j <= 3; j++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (x - j < 0)
        {
            break;
        }
        else if (omok_board_[Pos(x - j, y)] == omok_board_[Pos(x, y)])
        {
            left_mok++;
        }
        else if (omok_board_[Pos(x - j, y)] != Status::EMPTY)
        {
            break;
        }
    }

    mok += right_mok + left_mok;

    if (mok == 3 && x + right_mok < 17 && x - left_mok > 1)    //�� ������ 3���鼭 ���� ���� �پ������� �ȵȴ�
    {
        if ((omok_board_[Pos(x + right_mok + 2, y)] != Status::EMPTY && omok_board_[Pos(x + right_mok + 1, y)] != Status::EMPTY)
            || (omok_board_[Pos(x - left_mok - 2, y)] != Status::EMPTY && omok_board_[Pos(x - left_mok - 1, y)] != Status::EMPTY))
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }

    return 0;
}

int Game::CheckSamSamColumn(int x, int y)    // ���� (|) Ȯ��
{
    int mok = 1;
    int down_mok = 0;
    int up_mok = 0;
    int i, j;

    mok = 1;

    for (i = 1; i <= 3; i++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (y + i > 18)
        {
            break;
        }
        else if (omok_board_[Pos(x, y + i)] == omok_board_[Pos(x, y)])
        {
            down_mok++;
        }
        else if (omok_board_[Pos(x, y + i)] != Status::EMPTY)
        {
            break;
        }
    }

    for (j = 1; j <= 3; j++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (y - j < 0)
        {
            break;
        }
        else if (omok_board_[Pos(x, y - j)] == omok_board_[Pos(x, y)])
        {
            up_mok++;
        }
        else if (omok_board_[Pos(x, y - j)] != Status::EMPTY)
        {
            break;
        }
    }

    mok += down_mok + up_mok;

    if (mok == 3 && y + down_mok < 17 && y - up_mok > 1)    //�� ������ 3���鼭 ���� ���� �پ������� �ȵȴ�
    {
        if ((omok_board_[Pos(x, y + down_mok + 2)] != Status::EMPTY && omok_board_[Pos(x, y + down_mok + 1)] != Status::EMPTY)
            || (omok_board_[Pos(x, y - up_mok - 2)] != Status::EMPTY && omok_board_[Pos(x, y - up_mok - 1)] != Status::EMPTY))
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }

    return 0;
}

int Game::CheckSamSamSlash(int x, int y)    // �缱 (/) Ȯ��
{
    int mok = 1;
    int right_mok = 0;
    int left_mok = 0;
    int i, j;

    mok = 1;

    for (i = 1; i <= 3; i++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (x + i > 18 || y - i < 0)
        {
            break;
        }
        else if (omok_board_[Pos(x + i, y - i)] == omok_board_[Pos(x, y)])
        {
            right_mok++;
        }
        else if (omok_board_[Pos(x + i, y - i)] != Status::EMPTY)
        {
            break;
        }
    }

    for (j = 1; j <= 3; j++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (x - j < 0 || y + j > 18)
        {
            break;
        }
        else if (omok_board_[Pos(x - j, y + j)] == omok_board_[Pos(x, y)])
        {
            left_mok++;
        }
        else if (omok_board_[Pos(x - j, y + j)] != Status::EMPTY)
        {
            break;
        }
    }

    mok += right_mok + left_mok;

    if (mok == 3 && x + right_mok < 17 && y - right_mok > 1 && x - left_mok > 1 && y + left_mok < 17)    //�� ������ 3���鼭 ���� ���� �پ������� �ȵȴ�
    {
        if ((omok_board_[Pos(x + right_mok + 2, y - right_mok - 2)] != Status::EMPTY && omok_board_[Pos(x + right_mok + 1, y - right_mok - 1)] != Status::EMPTY) 
            || (omok_board_[Pos(x - left_mok -2, y + left_mok + 2)] != Status::EMPTY && omok_board_[Pos(x - left_mok - 1, y + left_mok + 1)] != Status::EMPTY))
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }

    return 0;
}

int Game::CheckSamSamBackSlash(int x, int y)    // ���缱 (��) Ȯ��
{
    int mok = 1;
    int right_mok = 0;
    int left_mok = 0;
    int i, j;

    mok = 1;

    for (i = 1; i <= 3; i++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (x + i > 18 || y + i > 18)
        {
            break;
        }
        else if (omok_board_[Pos(x + i, y + i)] == omok_board_[Pos(x, y)])
        {
            right_mok++;
        }
        else if (omok_board_[Pos(x + i, y + i)] != Status::EMPTY)
        {
            break;
        }
    }

    for (j = 1; j <= 3; j++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (x - j < 0 || y - j < 0)
        {
            break;
        }
        else if (omok_board_[Pos(x - j, y - j)] == omok_board_[Pos(x, y)])
        {
            left_mok++;
        }
        else if (omok_board_[Pos(x - j, y - j)] != Status::EMPTY)
        {
            break;
        }
    }

    mok += right_mok + left_mok;

    if (mok == 3 && x + right_mok < 17 && y + right_mok < 17 && x - left_mok > 1 && y - left_mok > 1)    //�� ������ 3���鼭 ���� ���� �پ������� �ȵȴ�
    {
        if ((omok_board_[Pos(x + right_mok + 2, y + right_mok + 2)] != Status::EMPTY && omok_board_[Pos(x + right_mok + 1, y + right_mok + 1)] != Status::EMPTY)
            || (omok_board_[Pos(x - left_mok - 2, y - left_mok - 2)] != Status::EMPTY && omok_board_[Pos(x - left_mok - 1, y - left_mok - 1)] != Status::EMPTY))
        {
            return 0;
        }
        else
        {
            return 1;
        }
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