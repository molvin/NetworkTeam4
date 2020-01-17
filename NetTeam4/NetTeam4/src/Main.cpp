#include <cstdlib>
#include "Networking/NetworkClient.h"
#include "Networking/BinaryStream.hpp"
#include "Engine/Engine.h"
#include "Game/Player.h"
#include "Server.h"
#include "Client.h"

#undef main

#define SERVER true

Server* server = nullptr;
Client* client = nullptr;

void temp()
{
	printf("Connection\n");
}

int main()
{	
	engineInit();

	if (SERVER)
	{
		server = new Server();
		std::string s;
		server->SocketClient.OnConnection = std::bind(&Server::OnConnect, server, s);
	}
	else
	{
		client = new Client();
		client->Join("10.20.3.132", 50000);
	}


	while(engIsOpen())
	{
		engineUpdate();

		if (SERVER)
		{
			server->Update();
		}
		else
		{
			client->Update();
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
