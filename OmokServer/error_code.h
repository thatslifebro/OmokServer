#pragma once

enum class ErrorCode
{
	InvalidSessionId = 1001,
	InvalidRoomId = 1002,
	NotLoggedIn = 1003,
	AlreadyInRoom = 1004,
	RoomGameStarted = 1005,
	NotInRoom = 1006,
};