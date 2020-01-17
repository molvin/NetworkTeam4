#include "Server.h"
#include "Game/Player.h"
#include "Engine/Engine.h"
#include "Engine/Key.h"
#include "Client.h"

Server::Server() : SocketClient(50000)
{
	SocketClient.RegisterMessage((Message*)new PlayerMessage(), MessageType::Player);
	SocketClient.RegisterMessage((Message*)new InputMessage(), MessageType::Input);

	SocketClient.Host();

}

void Server::Update(Player& player)
{
	this->player = &player;
	
	PlayerMessage playerMessage;
	playerMessage.x = player.x;
	playerMessage.y = player.y;
	SocketClient.AddMessageToQueue((Message*)&playerMessage, MessageType::Player);


	SocketClient.ReadData(*this);
	SocketClient.SendData();
}

void Server::UpdatePlayer(int x, int y)
{
	player->x = x;
	player->y = y;
}
