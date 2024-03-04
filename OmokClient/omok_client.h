#pragma once

#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketConnector.h"
#include "poco_session.h"

const Poco::UInt16 PORT = 32452;

class OmokClient
{
public:
	void Init();
};
