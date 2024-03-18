using System;
using System.Collections.Generic;
using System.Text;

namespace CSCommon
{
    // 1001 ~ 2000
    public class PacketID
    {
        
        public const UInt16 ReqLogin = 1001;
        public const UInt16 ResLogin = 1002;

        public const UInt16 NtfMustClose = 1005;

        public const UInt16 ReqRoomEnter = 1101;
        public const UInt16 ResRoomEnter = 1102;
        public const UInt16 NtfRoomUserList = 1103;
        public const UInt16 NtfRoomNewUser = 1104;

        public const UInt16 ReqRoomLeave = 1105;
        public const UInt16 ResRoomLeave = 1106;
        public const UInt16 NtfRoomLeaveUser = 1107;

        public const UInt16 NtfYouAreRoomAdmin = 1108;
        public const UInt16 NtfNewRoomAdmin = 1109;

        public const UInt16 ReqRoomChat = 1201;
        public const UInt16 ResRoomChat = 1202;
        public const UInt16 NtfRoomChat = 1203;

        public const UInt16 ReqMatch = 1301;
        public const UInt16 ResMatch = 1302;
        public const UInt16 NtfMatched = 1303;
        public const UInt16 NtfMatchReq = 1304;
        public const UInt16 ReqMatchRes = 1305;

        public const UInt16 ReqReadyOmok = 1401;
        public const UInt16 ResReadyOmok = 1402;
        public const UInt16 NtfStartOmok = 1403;

        public const UInt16 ReqPutMok = 1501;
        public const UInt16 ResPutMok = 1502;
        public const UInt16 NtfPutMok = 1503;
        public const UInt16 NtfEndOmok = 1504;

        public const UInt16 NtfMatchTimeout = 1601;
        public const UInt16 NtfReadyTimeout = 1602;
        public const UInt16 NtfPutMokTimeout = 1603;

        public const UInt16 NtfStartOmokView = 1701;
    }
}
