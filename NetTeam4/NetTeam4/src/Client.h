#pragma once
#include "Networking/NetworkClient.h"
#include "Game/Player.h"

class Client
{
public:
	Client();
	void Join(const std::string& ip, const int port);
	void Update(Player& player);

	NetworkClient SocketClient;
};

