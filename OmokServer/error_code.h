#pragma once

enum class ErrorCode
{
	None = 0,
	InvalidSessionId = 1001,
	InvalidRoomId = 1002,
	NotLoggedIn = 1003,
	AlreadyInRoom = 1004,
	RoomGameStarted = 1005,
	NotInRoom = 1006,
	AlreadyTryMatching = 1007,
	NotRoomAdmin = 1008,
	SameSessionIdWithOpponent = 1009,
	NotPlayer = 1010,
	GameAlreadyStarted = 1011,
	AlreadyReady = 1012,
	GameNotStarted = 1013,
	NotYourTurn = 1014,
	PutMokFail = 1015,

	TimeOut = 3001,
};