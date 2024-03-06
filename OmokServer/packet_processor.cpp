#include "packet_processor.h"
#include "packet_info.h"
#include "packet_id.h"

PacketProcessor::PacketProcessor() {
	packet_handler_map_.insert(std::make_pair(static_cast<uint16_t>(PacketId::ReqLogin), [&](Packet packet) { ReqLoginHandler(packet); }));
	//todo: 패킷에 대한 핸들러 등록
}

bool PacketProcessor::ProcessPacket()
{
	Packet packet = packet_queue_.PopAndGetPacket();
	if (packet.packet_body_size_ <= 0)
	{
		return false;
	}
	packet_handler_map_[packet.packet_id_](packet);
	return true;
}

void PacketProcessor::ReqLoginHandler(Packet packet)
{
	OmokPacket::ReqLogin reqLogin;
	reqLogin.ParseFromArray(packet.packet_body_, packet.packet_body_size_);

	std::print("받은 메시지 : reqLogin.userid = {}, pw = {}\n", reqLogin.userid(), reqLogin.pw());

	//todo : 로그인 처리 분리
	char sendBuf[1024] = { 0, };
	sendBuf[0] = 0x00;
	sendBuf[1] = 0x00;
	sendBuf[2] = (1002 & 0xFF);
	sendBuf[3] = (1002 >> 8) & 0xFF;

	short bufLength = 4;

	OmokPacket::ResLogin resLogin;
	resLogin.set_result(0);

	auto resLoginLength = resLogin.ByteSizeLong();
	resLogin.SerializeToArray(sendBuf + bufLength, resLoginLength);
	bufLength += resLoginLength;

	sendBuf[0] = (bufLength & 0xFF);
	sendBuf[1] = (bufLength >> 8) & 0xFF;

	packet.socket_->sendBytes(sendBuf, bufLength);
	std::print("Send to {} resLogin", reqLogin.userid());

}
