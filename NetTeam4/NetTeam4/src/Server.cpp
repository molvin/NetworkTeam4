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

void Server::Update()
{
	if (engGetKeyDown(Key::Escape))
		engClose();

	//SocketClient.AddMessageToQueue((Message*)playerMessage, MessageType::Player);
	
	SocketClient.ReadData(*this);
	SocketClient.SendData();
}

void Server::UpdatePlayer(int id, int x, int y)
{

}

void Server::OnConnect(std::string ip)
{
	printf("Connected: %s\n", ip.c_str());
	//TODO: give id to connection, spawn a player for connection
	ConnectionIdMessage* message = new ConnectionIdMessage();
	int id = IdCounter++;
	message->Id = id;

	SocketClient.SendData();
	SocketClient.AddMessageToQueue((Message*)message, MessageType::ConnectionId);
	SocketClient.SendData(ip);

	_players[id] = Player();
	_players[id].Id = id;
	_players[id].x = _players[id].y = id * 100;

	for (auto it : _players)
	{
		SpawnPlayerMessage* playerSpawnMessage = new SpawnPlayerMessage();
		playerSpawnMessage->OwnerId = it.first;
		playerSpawnMessage->x = it.second.x;
		playerSpawnMessage->y = it.second.y;
		SocketClient.AddMessageToQueue((Message*)playerSpawnMessage, MessageType::PlayerSpawnMessage);
	}	
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

void SpawnPlayerMessage::Read(BinaryStream* stream, NetworkManager& manager)
{
	OwnerId = stream->Read<int>();
	x = stream->Read<int>();
	y = stream->Read<int>();

	printf("Received player spawn message: id: %d, x: %d, y: %d\n", OwnerId, x, y);
}

int SpawnPlayerMessage::Write(BinaryStream* stream)
{
	stream->Write<int>(OwnerId);
	stream->Write<int>(x);
	stream->Write<int>(y);

	printf("Sending player spawn message\n");
	return sizeof(int) * 3;
}
