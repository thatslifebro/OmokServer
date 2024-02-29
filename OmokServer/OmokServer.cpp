#include <iostream>
#include <thread>
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/ParallelSocketAcceptor.h"
#include "PacketBufferManager.h"
#include "PacketManager.h"
#include "PacketInfo.h"
#include "PacketId.h"


using namespace Poco::Net;

constexpr Poco::UInt16 SERVER_PORT = 32452;

class Session
{
public:
	Session(StreamSocket& socket, SocketReactor& reactor) : _socket(socket), _reactor(reactor)
	{
		PacketBuffer = PacketBufferManager::GetInstance();

		_reactor.addEventHandler(_socket, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));
		_peerAddress = _socket.peerAddress().toString();
		std::cout << "Connection from " << _peerAddress << std::endl;

		_reactor.addEventHandler(_socket, Poco::Observer<Session, ShutdownNotification>(*this, &Session::onShutdown));
	}

	void onReadable(ReadableNotification* pNotification)
	{
		pNotification->release();
		try
		{
			char buffer[1024] = { 0, };
			
			int n = _socket.receiveBytes(buffer, sizeof(buffer));
			
			if (n > 0)
			{
				//OmokServer::Person person;
				std::cout << "Received from client " << std::endl;
				//person.ParseFromArray(buffer, n);
				//std::cout << "Received form client buffer : " << buffer << std::endl;
				//std::cout << "Received from client - person.id = " << person.id() << std::endl;
				//Thread::sleep(3000);
				//어떤 packet인지 헤더에서 확인
				//버퍼에서 헤더 잘라내기 + 나머지 처리하기
				//계속 packet을 job queue에 쌓아서 처리하는 방법이 있고
				//coroutine을 어떻게 쓸 수 있을까 생각해보자 => c++20 
				//코루틴
				//요청 처리
				if(PacketBuffer->Write(buffer, n)==false)
				{
					std::cout << "Fail PacketBuffer->Write" << std::endl;
				}
				std::cout << "success!" << std::endl;
				//응답
				//char szSendMessage[1024] = { 0, };
				////sprintf_s(szSendMessage, 1024 - 1, "Received Message: - id:  %d", person.id());
				//auto nMsgLen = strnlen_s(szSendMessage, 1024 - 1);
				//_socket.sendBytes(szSendMessage, nMsgLen);
			}
			else
			{
				_socket.shutdown();
				delete this;
			}
		}
		catch (Poco::Exception& exc)
		{
			_socket.shutdown();
			delete this;
		}
	}

	void onShutdown(ShutdownNotification* pNotification)
	{
		pNotification->release();
	}

	~Session()
	{
		_reactor.removeEventHandler(_socket, Poco::Observer<Session, ReadableNotification>(*this, &Session::onReadable));
		std::cout << "Connection from " << _peerAddress << " closed" << std::endl;
		_reactor.removeEventHandler(_socket, Poco::Observer<Session, ShutdownNotification
		>(*this, &Session::onShutdown));
	}

private:
	StreamSocket _socket;
	SocketReactor& _reactor;
	std::string _peerAddress;
	PacketBufferManager* PacketBuffer;
};

void PacketManage()
{
	PacketBufferManager* PacketBuffer = PacketBufferManager::GetInstance();
	PacketManager PacketProcessor;
	while (true)
	{
		auto packet = PacketBuffer->Read();
		if (packet != nullptr)
		{
			std::cout << "ProcessPacket" << std::endl;
			PacketProcessor.ProcessPacket(packet);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}

int main()
{	
	//패킷 버퍼 초기화
	PacketBufferManager* PacketBuffer = PacketBufferManager::GetInstance();
	PacketBuffer->Init(8096 * 10, PacketInfo::headerSize, PacketInfo::maxPacketSize);

	//패킷 처리 스레드
	std::jthread PacketManagerThread(PacketManage);

	//통신
	SocketReactor reactor;
	ServerSocket serverSocket(SERVER_PORT);
	ParallelSocketAcceptor<Session, SocketReactor> acceptor(serverSocket, reactor);
	reactor.run();
}

