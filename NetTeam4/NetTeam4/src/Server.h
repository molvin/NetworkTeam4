#pragma once
#include "Networking/NetworkClient.h"
#include "Game/Player.h"
#include "NetworkManager.h"

class Server : NetworkManager
{
public:
	int IdCounter = 0;
	Server();
	void Update();
	void UpdatePlayer(int id, int x, int y);
	void OnConnect(std::string ip);

	NetworkClient SocketClient;
	std::unordered_map<int, Player> _players;
};

class ConnectionIdMessage : Message
{
public:
	int Id;

	virtual void Read(BinaryStream* stream, NetworkManager& manager) override;

	virtual int Write(BinaryStream* stream) override;

};

class SpawnPlayerMessage : Message
{
	int OwnerId;
	int x, y;

	virtual void Read(BinaryStream* stream, NetworkManager& manager) override;

	virtual int Write(BinaryStream* stream) override;
};

