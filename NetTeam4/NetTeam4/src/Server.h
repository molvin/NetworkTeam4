#pragma once
#include "Networking/NetworkClient.h"

class Server
{
public:
	Server();
	void Update();

	NetworkClient SocketClient;
};

