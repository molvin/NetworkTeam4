#include <cstdlib>
#include "Networking/NetworkClient.h"
#include "Engine/Engine.h"
#include "Server.h"
#include "Client.h"

#undef main

#define SERVER 0
#define IP "192.168.0.103"

Server* server = nullptr;
Client* client = nullptr;

//TODO
//Handle connect when server isn't up yet
//Handle disconnect on server
//Handle disconnect on client

int main()
{	
	if (SERVER)
	{
		server = new Server();
		server->SocketClient.OnConnection = std::bind(&Server::OnConnect, server, std::placeholders::_1);
	}
	else
	{
		client = new Client();
		client->Join(IP, 50000);
		engineInit();
	}


	while(true)
	{
		if (SERVER)
		{
			server->Update();
		}
		else
		{
			client->Update();
			if (!engIsOpen())
				break;
		}

	}
	
	if(SERVER)
	{
		server->SocketClient.Close();
	}
	else
	{
		client->SocketClient.Close();
	}

	return 0;
}
