#pragma once
#include <vector>

#include "types.h"

class Game
{
public:
	Game() = default;
	Game(uint16_t white_session_id, uint32_t black_session_id) : white_session_id_(white_session_id), black_session_id_(black_session_id), turn_(Turn::BLACK_TURN), white_ready_(false), black_ready_(false), is_game_start_(false)
	{}

	void Init(uint32_t white_session_id, uint32_t black_session_id);

	bool SetStone(uint32_t x, uint32_t y, uint32_t session_id);
	
	void SetReady(uint32_t session_id);
	bool IsReady(uint32_t session_id);

	bool IsGameStarted();
	bool IsGameEnd();

	uint32_t GetOpponentId(uint32_t session_id);

	uint32_t GetWhiteSessionId();
	uint32_t GetBlackSessionId();

	uint32_t WinnerId();
	uint32_t LoserId();

	void ChangeTurn();

	void EndGame();

private:
	uint32_t white_session_id_;
	uint32_t black_session_id_;
	bool white_ready_;
	bool black_ready_;
	bool is_game_start_;
	Turn turn_;

	std::vector<Status> omok_board_;

	bool SetStone(uint32_t x, uint32_t y, Status color);

	bool CheckTurn(uint32_t session_id);

	bool CheckOmok(int x, int y);

	int 가로확인(int x, int y);
	int 세로확인(int x, int y);
	int 사선확인(int x, int y);
	int 역사선확인(int x, int y);

	bool CheckSamSam(int x, int y);

	int 가로삼삼확인(int x, int y);
	int 세로삼삼확인(int x, int y);
	int 사선삼삼확인(int x, int y);
	int 역사선삼삼확인(int x, int y);

	int Pos(int x, int y);
};
