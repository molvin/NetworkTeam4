#include "Server.h"

Server::Server() : _client(50000)
{
	_client.Host();
}

void Server::Update()
{
	_client.SendData();
}
