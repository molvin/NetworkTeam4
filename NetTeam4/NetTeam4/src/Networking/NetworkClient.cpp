#include "NetworkClient.h"
#include <iostream>
#include <errno.h>

NetworkClient::NetworkClient(int port) : _thread([=] { Listen(); }), _tempSendThread([=] {SendTemp(); })
{
	_port = port;
	WSADATA info;
	int error = WSAStartup(MAKEWORD(2, 0), &info);
	printf("Error code: %d\n", error);

	
	_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	printf("Socket: %d\n", _socket);
	if(_socket == INVALID_SOCKET)
	{
		printf("Socket failed to init: %d\n", WSAGetLastError());
	}
	
	SOCKADDR_IN address;


	address.sin_family = AF_INET;
	address.sin_port = htons(_port);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	int iResult = bind(_socket, (SOCKADDR*)& address, sizeof(address));
	if(iResult == -1)
	{
		printf("Failed to bind socket: %d\n", WSAGetLastError());
	}
 }


NetworkClient::~NetworkClient()
{
	Close();
}

void NetworkClient::Listen()
{
	while(true)
	{
		char buffer[1024];
		sockaddr_in sender;
		int size = sizeof(sender);
		int receiveResult = recvfrom(_socket, buffer, 1024, 0, (SOCKADDR*)&sender, &size);
		//Receive result is 0 on close, negative on error, otherwise the size of received buffer§

		if (receiveResult == -1)
		{
			int error = WSAGetLastError();
			printf("Listen Error: %d\n", error);				
		}
		else
		{
			printf("Received %d bytes \n", receiveResult);
			printf("%.*s\n", receiveResult, buffer);
		}
	}
}

void NetworkClient::SendTemp()
{
	while(true)
	{
		char buffer[] = { 'h', 'e', 'l', 'l', 'o' };
		sockaddr_in RecvAddr;
		RecvAddr.sin_family = AF_INET;
		RecvAddr.sin_port = htons(_port);
		RecvAddr.sin_addr.s_addr = inet_addr("10.20.3.3");
		int size = sizeof(buffer);
		int result = sendto(_socket, buffer, size, 0, (SOCKADDR*) &RecvAddr, sizeof(RecvAddr));
		if(result == -1)
		{
			int error = WSAGetLastError();
			printf("Send Error: %d\n", error);
		}
		else
		{
			//printf("Sent %d bytes\n", result);
		}
	}
}

void NetworkClient::Close()
{
	_thread.detach();
	_tempSendThread.detach();
	closesocket(_socket);
}
