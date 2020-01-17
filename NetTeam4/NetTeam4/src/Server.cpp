#include "Server.h"
#include "Game/Player.h"
#include "Engine/Engine.h"
#include "Engine/Key.h"

Server::Server() : SocketClient(50000)
{
	SocketClient.RegisterMessage((Message*)new PlayerMessage(), MessageType::Player);

	SocketClient.Host();

}

void Server::Update(Player& player)
{
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

	PlayerMessage playerMessage;
	playerMessage.x = player.x;
	playerMessage.y = player.y;
	SocketClient.AddMessageToQueue((Message*)&playerMessage, MessageType::Player);


	SocketClient.ReadData();
	SocketClient.SendData();
}
