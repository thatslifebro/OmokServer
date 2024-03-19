#pragma once
#include <vector>

#include "types.h"

class Game
{
public:
	void Init(uint32_t white_session_id, uint32_t black_session_id);

	bool IsReady(uint32_t session_id);
	void SetReady(uint32_t session_id);
	void StartGameIfBothReady();

	uint32_t GetWhiteSessionId() { return white_session_id_; }
	uint32_t GetBlackSessionId() { return black_session_id_; }

	bool CanSetStone(uint32_t x, uint32_t y, uint32_t session_id);
	void SetStone(uint32_t x, uint32_t y, uint32_t session_id);
	bool CheckOmok(int x, int y);
	void ChangeTurn();
	bool CheckTurn(uint32_t session_id);

	uint32_t WinnerId();
	uint32_t LoserId();

	bool IsGameStarted() { return is_game_start_; }
	void EndGame();

private:
	uint32_t white_session_id_ = 0;
	uint32_t black_session_id_ = 0;
	bool white_ready_ = false;
	bool black_ready_ = false;
	bool is_game_start_ = false;
	Turn turn_ = Turn::BLACK_TURN;

	std::vector<Status> omok_board_;

	bool CanSetStone(uint32_t x, uint32_t y, Status color);
	void SetStone(uint32_t x, uint32_t y, Status color);

	int CheckOmokRow(int x, int y);
	int CheckOmokColumn(int x, int y);
	int CheckOmokSlash(int x, int y);
	int CheckOmokBackSlash(int x, int y);

	bool CheckSamSam(int x, int y);

	int CheckSamSamRow(int x, int y);
	int CheckSamSamColumn(int x, int y);
	int CheckSamSamSlash(int x, int y);
	int CheckSamSamBackSlash(int x, int y);

	int Pos(int x, int y);
};
