enum class PacketId
{
	ReqLogin = 1001,
	ResLogin = 1002,

	ReqRoomEnter = 1101,
	ResRoomEnter = 1102,
	NtfRoomUserList = 1103,
	NtfRoomNewUser = 1104,
	ReqRoomLeave = 1105,
	ResRoomLeave = 1106,
	NtfRoomLeaveUser = 1107,

	ReqRoomChat = 1201,
	ResRoomChat = 1202,
	NtfRoomChat = 1203,

	ReqMatch = 1301,
	ResMatch = 1302,
	NtfMatched = 1303,

	ReqReadyOmok = 1401,
	ResReadyOmok = 1402,
	NtfStartOmok = 1403,

	ReqPutMok = 1501,
	ResPutMok = 1502,
	NtfPutMok = 1503,
	NtfEndOmok = 1504,
};