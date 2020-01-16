#include "NetworkClient.h"
#include <iostream>
#include <errno.h>
#include "BinaryStream.hpp"
#include "../Test/TestMessage.h"

NetworkClient::NetworkClient(int port) : _thread([=] { Listen(); })
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
	if (iResult == -1)
	{
		printf("Failed to bind socket: %d\n", WSAGetLastError());
	}
	else
		_bound = true;
 }

NetworkClient::~NetworkClient()
{
	Close();
}

void NetworkClient::Listen()
{
	while(true)
	{
		if (!_bound)
			continue;

		char buffer[65507];
		sockaddr_in sender;
		int size = sizeof(sender);
		int receiveResult = recvfrom(_socket, buffer, 65507, 0, (SOCKADDR*)&sender, &size);
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

			BinaryStream stream;			

			for(int i = 0; i < receiveResult; i++)
			{
				stream.Buffer.push_back(buffer[i]);
			}

			while (!stream.EndOfStream())
			{
				byte typeByte = stream.Read<byte>();
				printf("Type: %d\n", typeByte);
				_messages[(MessageType)typeByte]->Read(&stream);							
			}
			
		}
	}
}

void NetworkClient::SendData()
{
	BinaryStream stream;
	int size = 0;

	//stream.Write<byte>(_messageQueue.size());
	//size++;
	while (!_messageQueue.empty())
	{
		std::tuple<MessageType, Message*> tuple = _messageQueue.front();
		MessageType type = std::get<0>(tuple);
		Message* message = std::get<1>(tuple);

		//write the type 
		stream.Write<byte>((byte)type);
		//add size of type byte
		size++;
		//write message and add size
		size += message->Write(&stream);

		_messageQueue.pop();
	}
	if (size == 0)
	{
		return;
	}

	sockaddr_in RecvAddr;
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(_port);
	RecvAddr.sin_addr.s_addr = inet_addr("10.20.3.132");

	int result = sendto(_socket, (const char*)&stream.Buffer[0], size, 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
	if (result == -1)
	{
		int error = WSAGetLastError();
		printf("Send Error: %d\n", error);
	}
}

void NetworkClient::AddMessageToQueue(Message* message, MessageType type)
{
	_messageQueue.push({ type, message });
}

void NetworkClient::RegisterMessage(Message* message, MessageType type)
{
	//TODO: copy and make our own message, to ensure it is not destroyed
	_messages[type] = message;
}

void NetworkClient::Close()
{
	_thread.detach();
	closesocket(_socket);
}
