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
	void UpdatePlayer(int id, int x, int y, int frameId);
	void OnConnect(const std::string& ip);

	NetworkClient SocketClient;
	std::unordered_map<int, Player> _players;
	std::unordered_map<int, int> _processedFramesPerPlayer;
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
public:
	int OwnerId;
	float X, Y;

	virtual void Read(BinaryStream* stream, NetworkManager& manager) override;

	virtual int Write(BinaryStream* stream) override;
};

