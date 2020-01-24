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
	SocketClient.RegisterMessage((Message*)new SpawnPlayerMessage(), MessageType::PlayerSpawnMessage);

	SocketClient.Host();

}

void Server::Update()
{
	for (const auto it : _players)
	{
		PlayerMessage* playerMessage = new PlayerMessage();
		playerMessage->Id = it.first;
		playerMessage->X = it.second.Position.X;
		playerMessage->Y = it.second.Position.Y;
		playerMessage->FrameId = _processedFramesPerPlayer[it.first];
		SocketClient.AddMessageToQueue((Message*)playerMessage, MessageType::Player);
	}
	
	//Fake ping
	std::this_thread::sleep_for(std::chrono::milliseconds(100));	//100 ms
	SocketClient.ReadData(*this);
	SocketClient.SendData();
}

void Server::UpdatePlayer(const int id, const int x, const int y, const int frameId)
{
	if (_players.find(id) == _players.end())
		return;

	//Fake loss percentage
	if ((std::rand() % 1000) < 20)	//2%
		return;

	_players[id].Update(x, y, world);
	_processedFramesPerPlayer[id] = frameId;
}

void Server::OnConnect(const std::string& ip)
{
	printf("Connected: %s\n", ip.c_str());
	ConnectionIdMessage* message = new ConnectionIdMessage();
	const int id = IdCounter++;
	message->Id = id;

	SocketClient.SendData();
	SocketClient.AddMessageToQueue((Message*)message, MessageType::ConnectionId);
	SocketClient.SendData(ip);

	_players[id] = Player();
	_players[id].Id = id;
	_players[id].Position = Vector2(id * 100);
	_players[id].W = _players[id].H = 50;
	_processedFramesPerPlayer[id] = 0;

	for (const auto it : _players)
	{
		SpawnPlayerMessage* playerSpawnMessage = new SpawnPlayerMessage();
		playerSpawnMessage->OwnerId = it.first;
		playerSpawnMessage->X = it.second.Position.X;
		playerSpawnMessage->Y = it.second.Position.Y;
		SocketClient.AddMessageToQueue((Message*)playerSpawnMessage, MessageType::PlayerSpawnMessage);
	}	
}

//Messages

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
	X = stream->Read<float>();
	Y = stream->Read<float>();

	printf("Received player spawn message: id: %d, x: %f, y: %f\n", OwnerId, X, Y);

	Client& client = (Client&)manager;
	client.AddNewPlayer(OwnerId, X, Y);
}

int SpawnPlayerMessage::Write(BinaryStream* stream)
{
	stream->Write<int>(OwnerId);
	stream->Write<float>(X);
	stream->Write<float>(Y);

	printf("Sending player spawn message\n");
	return sizeof(int) * 3;
}
