#pragma once
#include "PacketManager.h"
#include "PacketInfo.h"
#include "PacketId.h"


PacketManager::PacketManager() {
	PacketBuffer = PacketBufferManager::GetInstance();
	packetHandlerMap.insert(std::make_pair(PacketId::ReqLogin, [&](const Packet& pkt) { ReqLoginHandler(pkt); }));
	//todo: ��Ŷ�� ���� �ڵ鷯 ���
}

void PacketManager::ProcessPacket(char* pktBuf)
{
	Packet pkt;
	pkt.packetSize = ((pktBuf[0] & 0xFF) << 8) | (pktBuf[1] & 0xFF);
	pkt.packetID = ((pktBuf[2] & 0xFF) << 8) | (pktBuf[3] & 0xFF);

    std::cout << "���� ��Ŷ ������ : " << pkt.packetSize << ", ��Ŷ ���̵� : " << pkt.packetID << std::endl;
	pkt.packetBody = pktBuf + PacketInfo::headerSize;
	std::cout << "ReqLoginHandler ȣ��" << std::endl;
	packetHandlerMap[pkt.packetID](pkt);
}

void PacketManager::ReqLoginHandler(const Packet& pkt)
{
	
	OmokPacket::ReqLogin reqLogin;
	reqLogin.ParseFromArray(pkt.packetBody, pkt.packetSize - PacketInfo::headerSize);

	std::cout << "���� �޽��� : reqLogin.userid = " << reqLogin.userid() << ", pw = " << reqLogin.pw() << std::endl;
}
