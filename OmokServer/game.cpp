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
    if (����Ȯ��(x, y) == 5)
    {
        is_game_start_ = false;
        return true;
    }

    else if (����Ȯ��(x, y) == 5)
    {
        is_game_start_ = false;
        return true;
    }

    else if (�缱Ȯ��(x, y) == 5)
    {
        is_game_start_ = false;
        return true;
    }

    else if (���缱Ȯ��(x, y) == 5)
    {
        is_game_start_ = false;
        return true;
    }

    return false;
}

#pragma region ����Ȯ��

int Game::����Ȯ��(int x, int y)      // �� Ȯ��
{
    int ���������� = 1;

    for (int i = 1; i <= 5; i++)
    {
        if (x + i <= 18 && omok_board_[Pos(x + i, y)] == omok_board_[Pos(x, y)])
            ����������++;

        else
            break;
    }

    for (int i = 1; i <= 5; i++)
    {
        if (x - i >= 0 && omok_board_[Pos(x - i, y)] == omok_board_[Pos(x, y)])
            ����������++;

        else
            break;
    }

    return ����������;
}

int Game::����Ȯ��(int x, int y)      // | Ȯ��
{
    int ���������� = 1;

    for (int i = 1; i <= 5; i++)
    {
        if (y + i <= 18 && omok_board_[Pos(x, y + i)] == omok_board_[Pos(x, y)])
            ����������++;

        else
            break;
    }

    for (int i = 1; i <= 5; i++)
    {
        if (y - i >= 0 && omok_board_[Pos(x, y - i)] == omok_board_[Pos(x, y)])
            ����������++;

        else
            break;
    }

    return ����������;
}

int Game::�缱Ȯ��(int x, int y)      // / Ȯ��
{
    int ���������� = 1;

    for (int i = 1; i <= 5; i++)
    {
        if (x + i <= 18 && y - i >= 0 && omok_board_[Pos(x + i, y - i)] == omok_board_[Pos(x, y)])
            ����������++;

        else
            break;
    }

    for (int i = 1; i <= 5; i++)
    {
        if (x - i >= 0 && y + i <= 18 && omok_board_[Pos(x - i, y + i)] == omok_board_[Pos(x, y)])
            ����������++;

        else
            break;
    }

    return ����������;
}

int Game::���缱Ȯ��(int x, int y)     // �� Ȯ��
{
    int ���������� = 1;

    for (int i = 1; i <= 5; i++)
    {
        if (x + i <= 18 && y + i <= 18 && omok_board_[Pos(x + i, y + i)] == omok_board_[Pos(x, y)])
            ����������++;

        else
            break;
    }

    for (int i = 1; i <= 5; i++)
    {
        if (x - i >= 0 && y - i >= 0 && omok_board_[Pos(x - i, y - i)] == omok_board_[Pos(x, y)])
            ����������++;

        else
            break;
    }

    return ����������;
}

#pragma endregion

bool Game::CheckSamSam(int x, int y)
{
	int check = 0;

    check += ���λ��Ȯ��(x, y);
    check += ���λ��Ȯ��(x, y);
    check += �缱���Ȯ��(x, y);
    check += ���缱���Ȯ��(x, y);

    if (check >= 2)
    {
        return true;
    }

    return false;
}

#pragma region ���Ȯ��

int Game::���λ��Ȯ��(int x, int y)    // ���� (��) Ȯ��
{
    int ��3��Ȯ�� = 1;
    int ��1 = 0;
    int ��2 = 0;
    int i, j;

    for (i = 1; i <= 3; i++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (x + i > 18)
            break;

        else if (omok_board_[Pos(x + i, y)] == omok_board_[Pos(x, y)])
            ��1++;

        else if (omok_board_[Pos(x + i, y)] != Status::EMPTY)
            break;
    }

    for (j = 1; j <= 3; j++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (x - j < 0)
            break;

        else if (omok_board_[Pos(x - j, y)] == omok_board_[Pos(x, y)])
            ��2++;

        else if (omok_board_[Pos(x - j, y)] != Status::EMPTY)
            break;
    }
    ��3��Ȯ�� += ��1 + ��2;

    if (��3��Ȯ�� == 3 && x + ��1 < 17 && x - ��2 > 1)    //�� ������ 3���鼭 ���� ���� �پ������� �ȵȴ�
    {
        if ((omok_board_[Pos(x + ��1 + 2, y)] != Status::EMPTY && omok_board_[Pos(x + ��1 + 1, y)] != Status::EMPTY) || (omok_board_[Pos(x - ��2 - 2, y)] != Status::EMPTY && omok_board_[Pos(x - ��2 - 1, y)] != Status::EMPTY))
        {
            return 0;
        }

        else
            return 1;
    }

    return 0;
}

int Game::���λ��Ȯ��(int x, int y)    // ���� (|) Ȯ��
{
    int ��3��Ȯ�� = 1;
    int ��1 = 0;
    int ��2 = 0;
    int i, j;

    ��3��Ȯ�� = 1;

    for (i = 1; i <= 3; i++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (y + i > 18)
            break;

        else if (omok_board_[Pos(x, y + i)] == omok_board_[Pos(x, y)])
            ��1++;

        else if (omok_board_[Pos(x, y + i)] != Status::EMPTY)
            break;
    }

    for (j = 1; j <= 3; j++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (y - j < 0)
            break;

        else if (omok_board_[Pos(x, y - j)] == omok_board_[Pos(x, y)])
            ��2++;

        else if (omok_board_[Pos(x, y - j)] != Status::EMPTY)
            break;
    }

    ��3��Ȯ�� += ��1 + ��2;

    if (��3��Ȯ�� == 3 && y + ��1 < 17 && y - ��2 > 1)    //�� ������ 3���鼭 ���� ���� �پ������� �ȵȴ�
    {
        if ((omok_board_[Pos(x, y + ��1 + 2)] != Status::EMPTY && omok_board_[Pos(x, y + ��1 + 1)] != Status::EMPTY) || (omok_board_[Pos(x, y - ��2 - 2)] != Status::EMPTY && omok_board_[Pos(x, y - ��2 - 1)] != Status::EMPTY))
        {
            return 0;
        }

        else
            return 1;
    }

    return 0;
}

int Game::�缱���Ȯ��(int x, int y)    // �缱 (/) Ȯ��
{
    int ��3��Ȯ�� = 1;
    int ��1 = 0;
    int ��2 = 0;
    int i, j;

    ��3��Ȯ�� = 1;

    for (i = 1; i <= 3; i++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (x + i > 18 || y - i < 0)
            break;

        else if (omok_board_[Pos(x + i, y - i)] == omok_board_[Pos(x, y)])
            ��1++;

        else if (omok_board_[Pos(x + i, y - i)] != Status::EMPTY)
            break;
    }

    for (j = 1; j <= 3; j++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (x - j < 0 || y + j > 18)
            break;

        else if (omok_board_[Pos(x - j, y + j)] == omok_board_[Pos(x, y)])
            ��2++;

        else if (omok_board_[Pos(x - j, y + j)] != Status::EMPTY)
            break;
    }

    ��3��Ȯ�� += ��1 + ��2;

    if (��3��Ȯ�� == 3 && x + ��1 < 17 && y - ��1 > 1 && x - ��2 > 1 && y + ��2 < 17)    //�� ������ 3���鼭 ���� ���� �پ������� �ȵȴ�
    {
        if ((omok_board_[Pos(x + ��1 + 2, y - ��1 - 2)] != Status::EMPTY && omok_board_[Pos(x + ��1 + 1, y - ��1 - 1)] != Status::EMPTY) || (omok_board_[Pos(x - ��2 -2, y + ��2 + 2)] != Status::EMPTY && omok_board_[Pos(x - ��2 - 1, y + ��2 + 1)] != Status::EMPTY))
        {
            return 0;
        }

        else
            return 1;
    }

    return 0;
}

int Game::���缱���Ȯ��(int x, int y)    // ���缱 (��) Ȯ��
{
    int ��3��Ȯ�� = 1;
    int ��1 = 0;
    int ��2 = 0;
    int i, j;

    ��3��Ȯ�� = 1;

    for (i = 1; i <= 3; i++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (x + i > 18 || y + i > 18)
            break;

        else if (omok_board_[Pos(x + i, y + i)] == omok_board_[Pos(x, y)])
            ��1++;

        else if (omok_board_[Pos(x + i, y + i)] != Status::EMPTY)
            break;
    }

    for (j = 1; j <= 3; j++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (x - j < 0 || y - j < 0)
            break;

        else if (omok_board_[Pos(x - j, y - j)] == omok_board_[Pos(x, y)])
            ��2++;

        else if (omok_board_[Pos(x - j, y - j)] != Status::EMPTY)
            break;
    }

    ��3��Ȯ�� += ��1 + ��2;

    if (��3��Ȯ�� == 3 && x + ��1 < 17 && y + ��1 < 17 && x - ��2 > 1 && y - ��2 > 1)    //�� ������ 3���鼭 ���� ���� �پ������� �ȵȴ�
    {
        if ((omok_board_[Pos(x + ��1 + 2, y + ��1 + 2)] != Status::EMPTY && omok_board_[Pos(x + ��1 + 1, y + ��1 + 1)] != Status::EMPTY) || (omok_board_[Pos(x - ��2 - 2, y - ��2 - 2)] != Status::EMPTY && omok_board_[Pos(x - ��2 - 1, y - ��2 - 1)] != Status::EMPTY))
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