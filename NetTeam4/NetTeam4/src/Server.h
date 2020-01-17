#pragma once
#include "Networking/NetworkClient.h"
#include "Game/Player.h"
#include "NetworkManager.h"

class Server : NetworkManager
{
public:
	Server();
	void Update(Player& player);
	void UpdatePlayer(int x, int y);

	NetworkClient SocketClient;
	Player* player = nullptr;
};

