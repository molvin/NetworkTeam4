#include <cstdlib>
#include "Networking/NetworkClient.h"
#include "Networking/BinaryStream.hpp"
#include "Test/TestMessage.h"
#include "Test/OtherTestMessage.h"
#include "Engine/Engine.h"
#include "Game/Player.h"
#include "Server.h"
#include "Client.h"

#undef main

#define SERVER false

Server* server = nullptr;
Client* client = nullptr;

int main()
{	
	engineInit();

	if (SERVER)
	{
		server = new Server();
	}
	else
	{
		client = new Client();
		client->Join("10.20.3.132", 50000);
	}



	Player player{ 0, 0, 50, 50 };
	PlayerMessage::player = &player;
	
	while(engIsOpen())
	{
		engineUpdate();

		//Player update
		engDrawRect(player.x, player.y, player.w, player.h);

		if (SERVER)
		{
			server->Update(player);
		}
		else
		{
			client->Update(player);
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
