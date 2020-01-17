#pragma once
#include "Networking/NetworkClient.h"

class Client
{
public:
	Client();
	void Join(const std::string& ip, const int port);
	void Update();

	NetworkClient SocketClient;
};

