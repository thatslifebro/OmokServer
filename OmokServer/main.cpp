#include "omok_server.h"
#include "flags.h"

int main(int argc, char** argv)
{	
	const flags::args args(argc, argv);

	OmokServer server;
	try
	{
		server.Init(args);
		server.Start();
	}
	catch (Poco::Exception& e)
	{
		std::print("PocoException: {}\n", e.displayText());
	}
	catch (std::exception& e)
	{
		std::print("Exception: {}\n", e.what());
	}
}

