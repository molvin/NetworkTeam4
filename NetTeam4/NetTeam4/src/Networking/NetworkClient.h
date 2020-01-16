#pragma once
#include <thread>
#include <WinSock2.h>

class NetworkClient
{
public:
	NetworkClient(int port);
	~NetworkClient();
	void Close();
	void SendData(unsigned char* buffer, int size);

private:
	void Listen();
	void SendTemp();
	
	std::thread _thread;
	std::thread _tempSendThread;
	SOCKET _socket;
	bool _bound = false;
	int _port;
};

