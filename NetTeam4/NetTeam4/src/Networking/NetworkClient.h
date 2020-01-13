#pragma once
#include <thread>
#include <WinSock2.h>

class NetworkClient
{
public:
	NetworkClient(int port);
	~NetworkClient();
	void Close();

private:
	void Listen();
	void SendTemp();
	
	std::thread _thread;
	std::thread _tempSendThread;
	SOCKET _socket;
	int _port;
};

