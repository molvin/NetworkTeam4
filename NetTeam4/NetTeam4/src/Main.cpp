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

void InitClient(NetworkClient& client)
{
	client.Join("10.20.3.132", 50000);
}
void InitServer(NetworkClient& server)
{
	server.Host();
}

void UpdateClient(NetworkClient& client)
{
	client.ReadData();
}
void UpdateServer(NetworkClient& server, Player& player)
{

	PlayerMessage playerMessage;
	playerMessage.x = player.x;
	playerMessage.y = player.y;
	server.AddMessageToQueue((Message*)&playerMessage, MessageType::Player);

	server.SendData();
}

#define SERVER true

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
		if (engGetKey(Key::W))
			player.y -= 1;			
		if (engGetKey(Key::S))
			player.y += 1;
		if (engGetKey(Key::A))
			player.x -= 1;
		if (engGetKey(Key::D))
			player.x += 1;

		if (engGetKeyDown(Key::Escape))
			engClose();

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
