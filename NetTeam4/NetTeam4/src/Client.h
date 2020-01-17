#pragma once
#include "Networking/NetworkClient.h"
#include "Game/Player.h"

class InputMessage : Message
{
public:
	int x, y;

	virtual void Read(BinaryStream* stream, NetworkManager& manager) override;
	virtual int Write(BinaryStream* stream) override;
};

class Client : NetworkManager
{
public:
	int Id = -1;
	Client();
	void Join(const std::string& ip, const int port);
	void Update(Player& player);

	NetworkClient SocketClient;
};

