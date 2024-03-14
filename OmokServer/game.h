#pragma once
#include <map>

//TODO 하나의 파일에 너무 많은 정의를 넣지 마세요. Game 클래스 이외는 다른 파일에서 정의하세요
struct Pos
{
	Pos(uint32_t x, uint32_t y) : x_(x), y_(y)
	{
	}
	uint32_t x_;
	uint32_t y_;
	
	bool operator < (const Pos& pos) const
	{
		return x_ < pos.x_ || (x_ == pos.x_ && y_ < pos.y_);
	}
};

//TODO enum class를 사용하세요
enum Turn
{
	BLACK_TURN,
	WHITE_TURN	
};

//TODO enum class를 사용하세요
enum status
{
	EMPTY,
	WHITE,
	BLACK
};

class Game
{
public:
	Game() = default;
	Game(uint16_t white_session_id, uint32_t black_session_id) : white_session_id_(white_session_id), black_session_id_(black_session_id), turn_(BLACK_TURN), white_ready_(false), black_ready_(false), is_game_start_(false)
	{}

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

private:
	uint32_t white_session_id_;
	uint32_t black_session_id_;
	bool white_ready_;
	bool black_ready_;
	bool is_game_start_;
	Turn turn_;

	//TODO std::map을 사용하지 마세요. std::array나 std::vector를 사용하세요
	std::map <Pos, status> omok_board_;

	bool SetStone(uint32_t x, uint32_t y, status color);

	bool CheckTurn(uint32_t session_id);

	bool CheckOmok(Pos pos);

	int 가로확인(int x, int y);
	int 세로확인(int x, int y);
	int 사선확인(int x, int y);
	int 역사선확인(int x, int y);

	bool CheckSamSam(Pos pos);

	int 가로삼삼확인(int x, int y);
	int 세로삼삼확인(int x, int y);
	int 사선삼삼확인(int x, int y);
	int 역사선삼삼확인(int x, int y);
};
