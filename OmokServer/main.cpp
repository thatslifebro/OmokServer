#include <iostream>
#include <thread>

#include "omok_server.h"

int main()
{	
	OmokServer server;
	server.Init();
	server.Start();
}

