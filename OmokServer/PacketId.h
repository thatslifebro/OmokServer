class PacketId
{
public:
	const static short ReqLogin = 1001;
	const static short ResLogin = 1002;

	const static short ReqRoomEnter = 1101;
	const static short ResRoomEnter = 1102;
	const static short NtfRoomUserList = 1103;
	const static short NtfRoomNewUser = 1104;
	const static short ReqRoomLeave = 1105;
	const static short ResRoomLeave = 1106;
	const static short NtfRoomLeaveUser = 1107;

	const static short ReqRoomChat = 1201;
	const static short ResRoomChat = 1202;
	const static short NtfRoomChat = 1203;

	const static short ReqMatch = 1301;
	const static short ResMatch = 1302;
	const static short NtfMatchUser = 1303;

	const static short ReqReadyOmok = 1401;
	const static short ResReadyOmok = 1402;
	const static short NtfStartOmok = 1403;

	const static short ReqPutMok = 1501;
	const static short ResPutMok = 1502;
	const static short NtfPutMok = 1503;
	const static short NtfEndOmok = 1504;
};