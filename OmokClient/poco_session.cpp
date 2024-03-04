#include "poco_session.h"

Session::Session(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor) : socket_(socket), reactor_(reactor)
{
	//이벤트 핸들러 등록
	reactor_.addEventHandler(socket_, Poco::Observer<Session, Poco::Net::ReadableNotification>(*this, &Session::onReadable));
	peer_address_ = socket_.peerAddress().toString();
	std::cout << "Connection from " << peer_address_ << std::endl;

	//시작시 메시지 보내기
	SendReqLogin("start", "startpw");
}

Session::~Session()
{
	try
	{
		//이벤트 핸들러 제거
		reactor_.removeEventHandler(socket_, Poco::Observer<Session, Poco::Net::ReadableNotification>(*this, &Session::onReadable));
		std::cout << "Connection from " << peer_address_ << " closed" << std::endl;
		reactor_.stop();
	}
	catch (Poco::Exception& exc)
	{
		std::cerr << "Session::~Session(): " << exc.displayText() << std::endl;
	}
}

void Session::onReadable(Poco::Net::ReadableNotification* pNf)
{
	pNf->release();
	char buffer[1024] = { 0, };
	auto n = socket_.receiveBytes(buffer, sizeof(buffer));
	if (n > 0)
	{
		if (send_count_ < 7)
		{
			send_count_++;
			SendReqLogin(std::to_string(send_count_), std::to_string(send_count_) + "pw");
		}
		else {
			socket_.close();
			delete this;
		}
	}
	else
	{
		delete this;
	}
}

void Session::SendReqLogin(std::string id, std::string pw)
{
	char sendBuf[1024] = { 0, };
	sendBuf[0] = 0x00;
	sendBuf[1] = 0x00;
	sendBuf[2] = (1001 >> 8) & 0xFF;
	sendBuf[3] = (1001 & 0xFF);

	short bufLength = 4;

	OmokPacket::ReqLogin reqLogin;
	reqLogin.set_userid(id);
	reqLogin.set_pw(pw);

	auto reqLoginLength = reqLogin.ByteSizeLong();
	reqLogin.SerializeToArray(sendBuf + bufLength, reqLoginLength);
	bufLength += reqLoginLength;

	sendBuf[0] = (bufLength >> 8);
	sendBuf[1] = (bufLength & 0xFF);

	socket_.sendBytes(sendBuf, bufLength);
	std::cout << "Send to server: Send Message" << std::endl;
}
