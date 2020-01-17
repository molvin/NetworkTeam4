#include <cstdlib>
#include "Networking/NetworkClient.h"
#include "Networking/BinaryStream.hpp"
#include "Test/TestMessage.h"
#include "Test/OtherTestMessage.h"
#include "Engine/Engine.h"
#include "Game/Player.h"
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


int main()
{	
	engineInit();

	NetworkClient client(50000);

	InitServer(client);
	//InitClient(client);

	client.RegisterMessage((Message*)new PlayerMessage(), MessageType::Player);

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

		
		//UpdateClient(temp);
		UpdateServer(client, player);
	}
	
	client.Close();	

	return 0;
}
