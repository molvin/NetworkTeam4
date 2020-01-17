#include "Client.h"
#include "Game/Player.h"

Client::Client() : SocketClient(60000)
{
	SocketClient.RegisterMessage((Message*)new PlayerMessage(), MessageType::Player);
}

void Client::Join(const std::string& ip, const int port)
{
	SocketClient.Join(ip, port);
}

void Client::Update()
{
	SocketClient.ReadData();
	SocketClient.SendData();
}
