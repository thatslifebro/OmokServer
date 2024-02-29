#include <iostream>
#include <string>
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketConnector.h"
#include "OmokPacket.pb.h"

using namespace Poco::Net;

constexpr Poco::UInt16 PORT = 32452;

class Session
{
public:
	Session(StreamSocket& socket, SocketReactor& reactor) : _socket(socket), _reactor(reactor)
	{
		_reactor.addEventHandler(_socket, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));
		_peerAddress = _socket.peerAddress().toString();
		std::cout << "Connection from " << _peerAddress << std::endl;

		for (int i = 0; i < 10; i++)
		{
			SendMessage();
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
		
		
	}

	~Session()
	{
		try
		{
			_reactor.removeEventHandler(_socket, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));
			std::cout << "Connection from " << _peerAddress << " closed" << std::endl;
			_reactor.stop();
		}
		catch (Poco::Exception& exc)
		{
			std::cerr << "Session::~Session(): " << exc.displayText() << std::endl;
		}
	}

	void onReadable(ReadableNotification* pNf)
	{
		pNf->release();
		char buffer[1024] = { 0, };
		auto n = _socket.receiveBytes(buffer, sizeof(buffer));
		if (n > 0)
		{
			//std::cout << "Received from server: " << buffer << std::endl;
			//std::cout << "Received from server: person.id = " << person.id() << std::endl;
			if (_sendCount < 7)
			{
				_sendCount++;
				SendMessage();
			}
			else {
				_socket.close();
				delete this;
			}
		}
		else
		{
			delete this;
		}
	}

	void SendMessage()
	{
		char sendBuf[1024] = { 0, };
		sendBuf[0] = 0x00;
		sendBuf[1] = 0x00;
		sendBuf[2] = (1001 >> 8) & 0xFF;
		sendBuf[3] = (1001 & 0xFF);

		short bufLength = 4;

		OmokPacket::ReqLogin reqLogin;
		reqLogin.set_userid("test");
		reqLogin.set_pw("testpw");

		auto reqLoginLength = reqLogin.ByteSizeLong();
		reqLogin.SerializeToArray(sendBuf + bufLength, reqLoginLength);
		bufLength += reqLoginLength;
		
		sendBuf[0] = (bufLength >> 8);
		sendBuf[1] = (bufLength & 0xFF);

		_socket.sendBytes(sendBuf, bufLength);
		std::cout<< "Send to server: Send Message" << std::endl;
	}



private:
	StreamSocket _socket;
	SocketReactor& _reactor;
	int _sendCount = 1;
	std::string _peerAddress;

};

int main()
{
	SocketReactor reactor;
	SocketAddress sa("localhost", PORT);
	SocketConnector<Session> connector(sa, reactor);

	reactor.run();
	getchar();
}
