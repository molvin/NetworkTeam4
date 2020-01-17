#pragma once
#include "Networking/NetworkClient.h"
#include "Game/Player.h"

class Client : NetworkManager
{
public:
	Client();
	void Join(const std::string& ip, const int port);
	void Update(Player& player);

	NetworkClient SocketClient;
	InputMessage message;
};

class InputMessage : Message
{
public:
	int x, y;

	virtual void Read(BinaryStream* stream, NetworkManager& manager) override;
	virtual int Write(BinaryStream* stream) override;
};