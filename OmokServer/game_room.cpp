#include "game_room.h"

void GameRoom::Init()
{
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
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
    else
    {
        return SetStone(x, y, BLACK);
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

bool GameRoom::SetStone(uint32_t x, uint32_t y, status color)
{
	Pos pos(x, y);
	if (omok_board_[pos] == EMPTY)
	{
        omok_board_[pos] = color;
		if (color == BLACK && CheckSamSam(pos))
		{
            omok_board_[pos] = EMPTY;
			return false;
		}
        else
        {
            CheckOmok(pos);
            ChangeTurn();
            return true;
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
    if (����Ȯ��(pos.x_, pos.y_) == 5)        // ���� �� ������ 5���� (6���̻��̸� ���� ���) 
    {
        is_game_start_ = false;
        return true;
    }

    else if (����Ȯ��(pos.x_, pos.y_) == 5)
    {
        is_game_start_ = false;
        return true;
    }

    else if (�缱Ȯ��(pos.x_, pos.y_) == 5)
    {
        is_game_start_ = false;
        return true;
    }

    else if (���缱Ȯ��(pos.x_, pos.y_) == 5)
    {
        is_game_start_ = false;
        return true;
    }

    return false;
}

int GameRoom::����Ȯ��(int x, int y)      // �� Ȯ��
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

int GameRoom::����Ȯ��(int x, int y)      // | Ȯ��
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

int GameRoom::�缱Ȯ��(int x, int y)      // / Ȯ��
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

int GameRoom::���缱Ȯ��(int x, int y)     // �� Ȯ��
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

bool GameRoom::CheckSamSam(Pos pos)
{
	int check = 0;

    check += ���λ��Ȯ��(pos.x_, pos.y_);
    check += ���λ��Ȯ��(pos.x_, pos.y_);
    check += �缱���Ȯ��(pos.x_, pos.y_);
    check += ���缱���Ȯ��(pos.x_, pos.y_);

    if (check >= 2)
    {
        return true;
    }

    return false;
}

int GameRoom::���λ��Ȯ��(int x, int y)    // ���� (��) Ȯ��
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

        else if (omok_board_[Pos(x + i, y)] != EMPTY)
            break;
    }

    for (j = 1; j <= 3; j++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (x - j < 0)
            break;

        else if (omok_board_[Pos(x - j, y)] == omok_board_[Pos(x, y)])
            ��2++;

        else if (omok_board_[Pos(x - j, y)] != EMPTY)
            break;
    }
    ��3��Ȯ�� += ��1 + ��2;

    if (��3��Ȯ�� == 3 && x + ��1 < 17 && x - ��2 > 1)    //�� ������ 3���鼭 ���� ���� �پ������� �ȵȴ�
    {
        if ((omok_board_[Pos(x + ��1 + 2, y)] != EMPTY && omok_board_[Pos(x + ��1 + 1, y)] != EMPTY) || (omok_board_[Pos(x - ��2 - 2, y)] != EMPTY && omok_board_[Pos(x - ��2 - 1, y)] != EMPTY))
        {
            return 0;
        }

        else
            return 1;
    }

    return 0;
}

int GameRoom::���λ��Ȯ��(int x, int y)    // ���� (|) Ȯ��
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

        else if (omok_board_[Pos(x, y + i)] != EMPTY)
            break;
    }

    for (j = 1; j <= 3; j++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (y - j < 0)
            break;

        else if (omok_board_[Pos(x, y - j)] == omok_board_[Pos(x, y)])
            ��2++;

        else if (omok_board_[Pos(x, y - j)] != EMPTY)
            break;
    }

    ��3��Ȯ�� += ��1 + ��2;

    if (��3��Ȯ�� == 3 && y + ��1 < 17 && y - ��2 > 1)    //�� ������ 3���鼭 ���� ���� �پ������� �ȵȴ�
    {
        if ((omok_board_[Pos(x, y + ��1 + 2)] != EMPTY && omok_board_[Pos(x, y + ��1 + 1)] != EMPTY) || (omok_board_[Pos(x, y - ��2 - 2)] != EMPTY && omok_board_[Pos(x, y - ��2 - 1)] != EMPTY))
        {
            return 0;
        }

        else
            return 1;
    }

    return 0;
}

int GameRoom::�缱���Ȯ��(int x, int y)    // �缱 (/) Ȯ��
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

        else if (omok_board_[Pos(x + i, y - i)] != EMPTY)
            break;
    }

    for (j = 1; j <= 3; j++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (x - j < 0 || y + j > 18)
            break;

        else if (omok_board_[Pos(x - j, y + j)] == omok_board_[Pos(x, y)])
            ��2++;

        else if (omok_board_[Pos(x - j, y + j)] != EMPTY)
            break;
    }

    ��3��Ȯ�� += ��1 + ��2;

    if (��3��Ȯ�� == 3 && x + ��1 < 17 && y - ��1 > 1 && x - ��2 > 1 && y + ��2 < 17)    //�� ������ 3���鼭 ���� ���� �پ������� �ȵȴ�
    {
        if ((omok_board_[Pos(x + ��1 + 2, y - ��1 - 2)] != EMPTY && omok_board_[Pos(x + ��1 + 1, y - ��1 - 1)] != EMPTY) || (omok_board_[Pos(x - ��2 -2, y + ��2 + 2)] != EMPTY && omok_board_[Pos(x - ��2 - 1, y + ��2 + 1)] != EMPTY))
        {
            return 0;
        }

        else
            return 1;
    }

    return 0;
}

int GameRoom::���缱���Ȯ��(int x, int y)    // ���缱 (��) Ȯ��
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

        else if (omok_board_[Pos(x + i, y + i)] != EMPTY)
            break;
    }

    for (j = 1; j <= 3; j++) // ���� �� ��ġ�κ��� �� Ȯ��
    {
        if (x - j < 0 || y - j < 0)
            break;

        else if (omok_board_[Pos(x - j, y - j)] == omok_board_[Pos(x, y)])
            ��2++;

        else if (omok_board_[Pos(x - j, y - j)] != EMPTY)
            break;
    }

    ��3��Ȯ�� += ��1 + ��2;

    if (��3��Ȯ�� == 3 && x + ��1 < 17 && y + ��1 < 17 && x - ��2 > 1 && y - ��2 > 1)    //�� ������ 3���鼭 ���� ���� �پ������� �ȵȴ�
    {
        if ((omok_board_[Pos(x + ��1 + 2, y + ��1 + 2)] != EMPTY && omok_board_[Pos(x + ��1 + 1, y + ��1 + 1)] != EMPTY) || (omok_board_[Pos(x - ��2 - 2, y - ��2 - 2)] != EMPTY && omok_board_[Pos(x - ��2 - 1, y - ��2 - 1)] != EMPTY))
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