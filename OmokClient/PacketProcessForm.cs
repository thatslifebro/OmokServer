using CSCommon;
using Google.Protobuf;
using OmokPacket;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace csharp_test_client
{
    public partial class mainForm
    {
        Dictionary<UInt16, Action<byte[]>> PacketFuncDic = new Dictionary<UInt16, Action<byte[]>>();

        Dictionary<string, int> RoomUserInfo = new Dictionary<string, int>();
        int mySessionID = 0;
        string myUserID = "";
        void SetPacketHandler()
        {
            //PacketFuncDic.Add(PACKET_ID.PACKET_ID_ERROR_NTF, PacketProcess_ErrorNotify);
            PacketFuncDic.Add(PacketID.ResLogin, PacketProcess_Loginin);

            PacketFuncDic.Add(PacketID.ResRoomEnter, PacketProcess_RoomEnterResponse);
            PacketFuncDic.Add(PacketID.NtfRoomAdmin, PacketProcess_RoomAdminNotify);
            PacketFuncDic.Add(PacketID.NtfNewRoomAdmin, PacketProcess_NewRoomAdminNotify);
            PacketFuncDic.Add(PacketID.NtfRoomUserList, PacketProcess_RoomUserListNotify);
            PacketFuncDic.Add(PacketID.NtfRoomNewUser, PacketProcess_RoomNewUserNotify);
            PacketFuncDic.Add(PacketID.ResRoomLeave, PacketProcess_RoomLeaveResponse);
            PacketFuncDic.Add(PacketID.NtfRoomLeaveUser, PacketProcess_RoomLeaveUserNotify);
            PacketFuncDic.Add(PacketID.ResRoomChat, PacketProcess_RoomChatResponse);
            PacketFuncDic.Add(PacketID.NtfRoomChat, PacketProcess_RoomChatNotify);
            PacketFuncDic.Add(PacketID.NtfMatchReq, PacketProcess_MatchReqNotify);
            PacketFuncDic.Add(PacketID.ResMatch, PacketProcess_MatchResponse);
            PacketFuncDic.Add(PacketID.NtfMatched, PacketProcess_MatchedNotify);

            PacketFuncDic.Add(PacketID.ResReadyOmok, PacketProcess_ReadyOmokResponse);
            PacketFuncDic.Add(PacketID.NtfStartOmok, PacketProcess_StartOmokNotify);
            PacketFuncDic.Add(PacketID.ResPutMok, PacketProcess_PutMokResponse);
            PacketFuncDic.Add(PacketID.NtfPutMok, PacketProcess_PutMokNotify);
            PacketFuncDic.Add(PacketID.NtfEndOmok, PacketProcess_EndOmokNotify);

        }

        void PacketProcess(PacketData packet)
        {
            var packetID = packet.PacketID;

            if (PacketFuncDic.ContainsKey(packetID))
            {
                PacketFuncDic[packetID](packet.BodyData);
            }
            else
            {
                DevLog.Write("Unknown Packet Id: " + packet.PacketID.ToString());
            }
        }

        void PacketProcess_PutStoneInfoNotifyResponse(byte[] bodyData)
        {
            /*var responsePkt = new PutStoneNtfPacket();
            responsePkt.FromBytes(bodyData);

            DevLog.Write($"'{responsePkt.userID}' Put Stone  : [{responsePkt.xPos}] , [{responsePkt.yPos}] ");*/

        }

        void PacketProcess_GameStartResultResponse(byte[] bodyData)
        {
            /*var responsePkt = new GameStartResPacket();
            responsePkt.FromBytes(bodyData);

            if ((ERROR_CODE)responsePkt.Result == ERROR_CODE.NOT_READY_EXIST)
            {
                DevLog.Write($"모두 레디상태여야 시작합니다.");
            }
            else
            {
                DevLog.Write($"게임시작 !!!! '{responsePkt.UserID}' turn  ");
            }*/
        }

        void PacketProcess_GameEndResultResponse(byte[] bodyData)
        {
            /*var responsePkt = new GameResultResPacket();
            responsePkt.FromBytes(bodyData);
            
            DevLog.Write($"'{responsePkt.UserID}' WIN , END GAME ");*/

        }

        void PacketProcess_PutStoneResponse(byte[] bodyData)
        {
            /*var responsePkt = new MatchUserResPacket();
            responsePkt.FromBytes(bodyData);

            if((ERROR_CODE)responsePkt.Result != ERROR_CODE.ERROR_NONE)
            {
                DevLog.Write($"Put Stone Error : {(ERROR_CODE)responsePkt.Result}");
            }

            DevLog.Write($"다음 턴 :  {(ERROR_CODE)responsePkt.Result}");*/

        }




        void PacketProcess_ErrorNotify(byte[] bodyData)
        {
            /*var notifyPkt = new ErrorNtfPacket();
            notifyPkt.FromBytes(bodyData);

            DevLog.Write($"에러 통보 받음:  {notifyPkt.Error}");*/
        }


        void PacketProcess_Loginin(byte[] bodyData)
        {
            var reslogin = new ResLogin();
            reslogin.MergeFrom(bodyData);
            DevLog.Write($"로그인 결과: {reslogin.Result}");
        }

        void PacketProcess_RoomEnterResponse(byte[] bodyData)
        {
            var resRoomEnter = new ResRoomEnter();
            resRoomEnter.MergeFrom(bodyData);
            DevLog.Write($"방 입장 결과:  {resRoomEnter.Result}");

            myUserID = resRoomEnter.UserInfo.UserId;
            mySessionID = resRoomEnter.UserInfo.SessionId;

            if(resRoomEnter.Result == 0)
            {
                listBoxRoomUserList.Items.Add(resRoomEnter.UserInfo.UserId);
                RoomUserInfo[resRoomEnter.UserInfo.UserId] = resRoomEnter.UserInfo.SessionId;
            }

        }

        void PacketProcess_RoomAdminNotify(byte[] bodyData)
        {
            AdminToTop(myUserID);
            DevLog.Write("당신이 방장입니다.");
        }

        void AdminToTop(string userId)
        {
            listBoxRoomUserList.Items.Remove(userId);
            object[] items = new object[listBoxRoomUserList.Items.Count];
            listBoxRoomUserList.Items.CopyTo(items, 0);
            listBoxRoomUserList.Items.Clear();
            listBoxRoomUserList.Items.Add(userId);
            foreach (var item in items)
            {
                listBoxRoomUserList.Items.Add(item);
            }
        }

        void PacketProcess_NewRoomAdminNotify(byte[] bodyData)
        {
            var ntfNewRoomAdmin = new NtfNewRoomAdmin();
            ntfNewRoomAdmin.MergeFrom(bodyData);
            var user = ntfNewRoomAdmin.UserInfo;
            DevLog.Write($"새로운 방장: {user.UserId}");

            AdminToTop(user.UserId);
        }

        void PacketProcess_RoomUserListNotify(byte[] bodyData)
        {
            var ntfRoomUserList = new NtfRoomUserList();
            ntfRoomUserList.MergeFrom(bodyData);
            foreach (var userinfo in ntfRoomUserList.UserInfo)
            {
                RoomUserInfo.Add(userinfo.UserId, userinfo.SessionId);
                listBoxRoomUserList.Items.Add(userinfo.UserId);
            }
            DevLog.Write($"방의 기존 유저 리스트 받음");
        }

        void PacketProcess_RoomNewUserNotify(byte[] bodyData)
        {
            NtfRoomNewUser ntfRoomNewUser = new NtfRoomNewUser();
            ntfRoomNewUser.MergeFrom(bodyData);

            RoomUserInfo.Add(ntfRoomNewUser.UserInfo.UserId, ntfRoomNewUser.UserInfo.SessionId);
            listBoxRoomUserList.Items.Add(ntfRoomNewUser.UserInfo.UserId);

            DevLog.Write($"방에 새로 들어온 유저 받음");
        }


        void PacketProcess_RoomLeaveResponse(byte[] bodyData)
        {
            var resRoomLeave = new ResRoomLeave();
            resRoomLeave.MergeFrom(bodyData);
            DevLog.Write($"방 나감 결과:  {resRoomLeave.Result}");
            if(resRoomLeave.Result == 0)
            {
                listBoxRoomUserList.Items.Clear();
                listBoxRoomChatMsg.Items.Clear();
                RoomUserInfo.Clear();
            }
        }

        void PacketProcess_RoomLeaveUserNotify(byte[] bodyData)
        {
            var ntfRoomLeaveUser = new NtfRoomLeaveUser();
            ntfRoomLeaveUser.MergeFrom(bodyData);
            listBoxRoomUserList.Items.Remove(ntfRoomLeaveUser.UserInfo.UserId);

            RoomUserInfo.Remove(ntfRoomLeaveUser.UserInfo.UserId);
            DevLog.Write($"방에서 유저 나감");
        }


        void PacketProcess_RoomChatResponse(byte[] bodyData)
        {
            var resRoomChat = new ResRoomChat();
            resRoomChat.MergeFrom(bodyData);
            if(resRoomChat.Result == 0)
            {
                AddRoomChatMessageList(textBoxUserID.Text, resRoomChat.Chat);
            }
            else
            {
                DevLog.Write($"방 채팅 보내기: 실패");
            }
        }


        void PacketProcess_RoomChatNotify(byte[] bodyData)
        {
            var ntfRoomChat = new NtfRoomChat();
            ntfRoomChat.MergeFrom(bodyData);
            AddRoomChatMessageList(ntfRoomChat.UserInfo.UserId, ntfRoomChat.Chat);
        }

        void PacketProcess_MatchResponse(byte[] bodyData)
        {
            var resMatch = new ResMatch();
            resMatch.MergeFrom(bodyData);
            if(resMatch.Result == 0)
            {
                DevLog.Write("상대가 요청에 수락");
                DevLog.Write("Game Ready 버튼을 눌러 게임을 시작하세요.");
            }
            else
            {
                DevLog.Write("상대가 요청에 거절");
            }
        }

        void PacketProcess_MatchReqNotify(byte[] bodyData)
        {
            var ntfMatchReq = new NtfMatchReq();
            ntfMatchReq.MergeFrom(bodyData);

            DevLog.Write($"매칭 요청 받음: {ntfMatchReq.UserInfo.UserId}");

            OmokClient.PopUp popUp = new();
            popUp.DataPassEvent += new OmokClient.PopUp.DataPassEventHandler(DataReceiveEvent);
            popUp.ChangeLabel(ntfMatchReq.UserInfo.UserId);
            popUp.ShowDialog();
        }

        
        void PacketProcess_MatchedNotify(byte[] bodyData)
        {
            //var ntfMatched = new NtfMatched();
            //ntfMatched.MergeFrom(bodyData);
            
            //DevLog.Write($"매칭 완료 \n\n 상대 : {ntfMatched.UserId}\n 준비를 눌러 게임을 시작하십시오.\n");
        }

        void AddRoomChatMessageList(string userID, string message)
        {
            if (listBoxRoomChatMsg.Items.Count > 512)
            {
                listBoxRoomChatMsg.Items.Clear();
            }

            listBoxRoomChatMsg.Items.Add($"[{userID}]: {message}");
            listBoxRoomChatMsg.SelectedIndex = listBoxRoomChatMsg.Items.Count - 1;
        }

        void PacketProcess_ReadyOmokResponse(byte[] bodyData)
        {
            DevLog.Write("준비 완료 응답");
        }

        void PacketProcess_StartOmokNotify(byte[] bodyData)
        {
            var ntfStartOmok = new NtfStartOmok();
            ntfStartOmok.MergeFrom(bodyData);

            var isMyTurn = ntfStartOmok.Black;
            var myID = ntfStartOmok.MyId;
            var opponentID = ntfStartOmok.OpponentId;
            
            StartGame(isMyTurn, myID, opponentID);

            if(isMyTurn)
            {
                DevLog.Write($"게임 시작. 흑돌 플레이어: {myID}, 백돌 플레이어: {opponentID}");
            }
            else
            {
                DevLog.Write($"게임 시작. 백돌 플레이어: {myID}, 흑돌 플레이어: {opponentID}");
            }
        }
        

        void PacketProcess_PutMokResponse(byte[] bodyData)
        {
            var resPutMok = new ResPutMok();
            resPutMok.MergeFrom(bodyData);

            if(resPutMok.Result == 0)
            {
                DevLog.Write($"오목 놓기 성공");
            }
            else
            {
                DevLog.Write($"오목 놓기 실패");
                무르기();
            }
        }
        

        void PacketProcess_PutMokNotify(byte[] bodyData)
        {
            var ntfPutMok = new NtfPutMok();
            ntfPutMok.MergeFrom(bodyData);

            var x = ntfPutMok.X;
            var y = ntfPutMok.Y;

            플레이어_돌두기(true, x, y);

            DevLog.Write($"상대방이 [{x}, {y}] 에 돌을 놓았습니다.");
        }


        void PacketProcess_EndOmokNotify(byte[] bodyData)
        {
            var ntfEndOmok = new NtfEndOmok();
            ntfEndOmok.MergeFrom(bodyData);

            EndGame();

            if(ntfEndOmok.Status==1)
            {
                DevLog.Write($"오목 GameOver: Win");
            }
            else if(ntfEndOmok.Status==0)
            {
                DevLog.Write($"오목 GameOver: Lose");
            }
            else if(ntfEndOmok.Status==2)
            {
                DevLog.Write($"오목 GameOver: Draw");
            }
            else
            {
                DevLog.Write($"오목 GameOver: 상대방의 연결이 끊겼습니다. Win");
            }
        }
    }
}
