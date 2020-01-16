#include <cstdlib>
#include "Networking/NetworkClient.h"
#include "Networking/BinaryStream.hpp"
#include "Test/TestMessage.h"
#include "Test/OtherTestMessage.h"
#include "Engine/Engine.h"
#include "Game/Player.h"
#undef main

int main()
{	
	engineInit();

	TestMessage message0;
	message0.TestInt = 12;
	OtherTestMessage message1;
	message1.Value = 3.14f;

	TestMessage* reg = new TestMessage();
	OtherTestMessage* regother = new OtherTestMessage();

	NetworkClient client(50000);
	NetworkClient server(60000);

	server.RegisterMessage((Message*)reg, MessageType::Test);
	client.RegisterMessage((Message*)reg, MessageType::Test);
	server.RegisterMessage((Message*)regother, MessageType::OtherTest);
	client.RegisterMessage((Message*)regother, MessageType::OtherTest);
	server.RegisterMessage((Message*)new PlayerMessage(), MessageType::Player);
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
		//server.AddMessageToQueue((Message*)&message0, MessageType::Test);
		//server.AddMessageToQueue((Message*)&message1, MessageType::OtherTest);
		PlayerMessage playerMessage;
		playerMessage.x = player.x;
		playerMessage.y = player.y;
		server.AddMessageToQueue((Message*)&playerMessage, MessageType::Player);
		
		server.SendData();
	}
	
	client.Close();	
	server.Close();

	return 0;
}
