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
	printf("Connected\n");
	//TODO: give id to connection, spawn a player for connection
	ConnectionIdMessage* message = new ConnectionIdMessage();
	message->Id = IdCounter++;

	SocketClient.SendData();
	SocketClient.AddMessageToQueue((Message*)message, MessageType::ConnectionId);
	SocketClient.SendData(ip);
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
}

int SpawnPlayerMessage::Write(BinaryStream* stream)
{
	return 0;
}
