#include "Server.h"

#include <NetworkLayer.h>
#include <iostream>


void Server::Run()
{
	EnetServer* s = new EnetServer();
	auto result = s->init("127.0.0.1", "1234", 2);

	if (result == false)
	{
		std::cout << "Server could not start (it's probably running already)" << std::endl;
		return;
	}

	Packet data;
	while (1)
	{
		data = s->receive();
		s->broadcast(data);
	}

	return;
}
