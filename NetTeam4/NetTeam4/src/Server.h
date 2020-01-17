#pragma once
#include "Networking/NetworkClient.h"
#include "Game/Player.h"
#include "NetworkManager.h"

class Server : NetworkManager
{
public:
	int IdCounter = 0;
	Server();
	void Update(Player& player);
	void UpdatePlayer(int x, int y);
	void OnConnect();

	NetworkClient SocketClient;
	Player* player = nullptr;
};

class ConnectionIdMessage : Message
{
public:
	int Id;

	virtual void Read(BinaryStream* stream, NetworkManager& manager) override;

	virtual int Write(BinaryStream* stream) override;

};

