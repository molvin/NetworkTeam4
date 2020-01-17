#pragma once
#include "Networking/NetworkClient.h"
#include "Game/Player.h"

class Server
{
public:
	Server();
	void Update(Player& player);

	NetworkClient SocketClient;
};

