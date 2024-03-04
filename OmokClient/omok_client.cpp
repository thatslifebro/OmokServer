#include "omok_client.h"


void OmokClient::Init()
{
	Poco::Net::SocketReactor reactor;
	Poco::Net::SocketAddress sa("localhost", PORT);
	Poco::Net::SocketConnector<Session> connector(sa, reactor);
	reactor.run();
}