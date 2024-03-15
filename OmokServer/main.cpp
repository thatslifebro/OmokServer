#include "omok_server.h"
#include "flags.h"

int main(int argc, char** argv)
{	
	const flags::args args(argc, argv);

	OmokServer server;
	server.Init(args);
	server.Start();
}

