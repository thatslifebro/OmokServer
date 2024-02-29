#pragma once
#include "PacketManager.h"
#include "PacketInfo.h"
#include "PacketId.h"


PacketManager::PacketManager() {
	PacketBuffer = PacketBufferManager::GetInstance();
	packetHandlerMap.insert(std::make_pair(PacketId::ReqLogin, [&](const Packet& pkt) { ReqLoginHandler(pkt); }));
	//todo: 패킷에 대한 핸들러 등록
}

void PacketManager::ProcessPacket(char* pktBuf)
{
	Packet pkt;
	pkt.packetSize = ((pktBuf[0] & 0xFF) << 8) | (pktBuf[1] & 0xFF);
	pkt.packetID = ((pktBuf[2] & 0xFF) << 8) | (pktBuf[3] & 0xFF);

    std::cout << "받은 패킷 사이즈 : " << pkt.packetSize << ", 패킷 아이디 : " << pkt.packetID << std::endl;
	pkt.packetBody = pktBuf + PacketInfo::headerSize;
	std::cout << "ReqLoginHandler 호출" << std::endl;
	packetHandlerMap[pkt.packetID](pkt);
}

void PacketManager::ReqLoginHandler(const Packet& pkt)
{
	
	OmokPacket::ReqLogin reqLogin;
	reqLogin.ParseFromArray(pkt.packetBody, pkt.packetSize - PacketInfo::headerSize);

	std::cout << "받은 메시지 : reqLogin.userid = " << reqLogin.userid() << ", pw = " << reqLogin.pw() << std::endl;
}
