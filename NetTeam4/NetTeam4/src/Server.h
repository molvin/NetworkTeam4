#pragma once
#include "Networking/NetworkClient.h"

class Server
{
public:
	Server();
	void Update();
private:
	NetworkClient _client;
};

