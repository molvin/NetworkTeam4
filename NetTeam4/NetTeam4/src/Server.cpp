#include "Server.h"
#include "Game/Player.h"
#include "Engine/Engine.h"
#include "Engine/Key.h"
#include "Client.h"

Server::Server() : SocketClient(50000)
{
	SocketClient.RegisterMessage((Message*)new PlayerMessage(), MessageType::Player);
	SocketClient.RegisterMessage((Message*)new InputMessage(), MessageType::Input);
	SocketClient.RegisterMessage((Message*)new ConnectionIdMessage(), MessageType::ConnectionId);

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
	player->x += x;
	player->y += y;
}

void Server::OnConnect()
{
	printf("Connected\n");
	//TODO: give id to connection, spawn a player for connection
	std::shared_ptr<ConnectionIdMessage> message = std::make_shared<ConnectionIdMessage>();
	message->Id = IdCounter++;

	SocketClient.AddMessageToQueue((Message*) &message, MessageType::ConnectionId);	
}

void ConnectionIdMessage::Read(BinaryStream* stream, NetworkManager& manager)
{
	Id = stream->Read<int>();
	Client& client = (Client&)manager;
	client.Id = Id;
	printf("Received Id from server: %d\n", Id);
}

int ConnectionIdMessage::Write(BinaryStream* stream)
{
	stream->Write<int>(Id);
	return sizeof(int);
}
