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
	while(!_closeThread)
	{
		if (!_bound)
			continue;

		char buffer[1024];
		sockaddr_in sender;
		int size = sizeof(sender);
		int receiveResult = recvfrom(_socket, buffer, 1024, 0, (SOCKADDR*)&sender, &size);
		//Receive result is 0 on close, negative on error, otherwise the size of received buffer

		if (receiveResult == -1)
		{
			int error = WSAGetLastError();
			printf("Listen Error: %d\n", error);	
			return;
		}

		std::string ip = inet_ntoa(sender.sin_addr);
		if (_hosting && _connections.find(ip) == _connections.end())
		{
			//Check if host should add known connection
			if (receiveResult == 5 && buffer[0] == (unsigned char)MessageType::Join)
			{
				BinaryStream stream;
				for (int i = 0; i < receiveResult; i++)
					stream.Buffer.push_back(buffer[i]);
				stream.Read<byte>();
				int port = stream.Read<int>();
				_connections[ip] = Connection{ ip, port };
				printf("Added new connection %s, %d\n", ip.c_str(), port);
				return;
			}
		}
		BinaryStream stream;

		for (int i = 0; i < receiveResult; i++){
			stream.Buffer.push_back(buffer[i]);
		}

		while (!stream.EndOfStream()){
			byte typeByte = stream.Read<byte>();
			printf("Type: %d\n", typeByte);
			_messages[(MessageType)typeByte]->Read(&stream);
		}
			
		
	}
	printf("Closing thread");
}

void NetworkClient::SendData()
{
	BinaryStream stream;
	int size = 0;

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

	for (auto it : _connections)	
	{
		sockaddr_in recvAddr;
		recvAddr.sin_family = AF_INET;
		recvAddr.sin_port = htons(it.second.Port);
		recvAddr.sin_addr.s_addr = inet_addr(it.second.Ip.c_str());

		int result = sendto(_socket, (const char*)&stream.Buffer[0], size, 0, (SOCKADDR*)&recvAddr, sizeof(recvAddr));
		if (result == -1){
			int error = WSAGetLastError();
			printf("Send Error: %d\n", error);
		}
		else
		{
			//printf("Sent %d bytes to %s, with port %d\n", size, it.second.Ip.c_str(), it.second.Port);
		}
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

void NetworkClient::Join(std::string ip, int port)
{
	_hosting = false;
	_connections[ip] = Connection{ ip, port };
	BinaryStream stream;
	stream.Write<byte>((byte)MessageType::Join);
	stream.Write<int>(_port);

	sockaddr_in RecvAddr;
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(port);
	RecvAddr.sin_addr.s_addr = inet_addr(ip.c_str());
	int result = sendto(_socket, (const char*)&stream.Buffer[0], sizeof(byte) + sizeof(int), 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
}

void NetworkClient::Host()
{
	_hosting = true;
}

void NetworkClient::Close()
{
	_closeThread = true;
	closesocket(_socket);
	while (_thread.joinable())	
	{
		_thread.join();
	}
}
