#pragma once
#include "Networking/NetworkClient.h"
#include "Game/Player.h"

class InputMessage : Message
{
public:
	int id, x, y;

	virtual void Read(BinaryStream* stream, NetworkManager& manager) override;
	virtual int Write(BinaryStream* stream) override;
};

class Client : NetworkManager
{
public:
	int Id = -1;
	Client();
	void Join(const std::string& ip, const int port);
	void Update();

	NetworkClient SocketClient;
	std::unordered_map<int, Player> _players;
};

