#include "Server.h"
#include "Game/Player.h"

Server::Server() : SocketClient(50000)
{
	SocketClient.RegisterMessage((Message*)new PlayerMessage(), MessageType::Player);

	SocketClient.Host();

}

void Server::Update()
{
	SocketClient.ReadData();
	SocketClient.SendData();
}
