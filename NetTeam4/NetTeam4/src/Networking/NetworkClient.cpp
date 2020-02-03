#include "NetworkClient.h"
#include <iostream>
#include <errno.h>
#include <mutex>
#include "BinaryStream.hpp"

NetworkClient::NetworkClient(int port) : _thread([=] { Listen(); })
{
	_port = port;
	WSADATA info;
	int error = WSAStartup(MAKEWORD(2, 0), &info);
	printf("Error code: %d\n", error);


	_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	printf("Socket: %d\n", _socket);
	if (_socket == INVALID_SOCKET)
	{
		printf("Socket failed to init: %d\n", WSAGetLastError());
	}


	int iResult = -1;
	int count = 0;
	do
	{
		SOCKADDR_IN address;

		address.sin_family = AF_INET;
		address.sin_port = htons(_port + count);
		address.sin_addr.s_addr = htonl(INADDR_ANY);

		iResult = bind(_socket, (SOCKADDR*)& address, sizeof(address));
		if (iResult == -1)
		{
			printf("Failed to bind socket: %d\n", WSAGetLastError());
			count++;
		}
		else
		{
			_port = _port + count;
			_bound = true;
			printf("Bound socket successfully at port: %d\n", _port);
		}
	} while (iResult == -1 && count < 100);
	
}

NetworkClient::~NetworkClient()
{
	Close();
}

void NetworkClient::Listen()
{
	while (!_closeThread)
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
			continue;
		}

		std::string ip = inet_ntoa(sender.sin_addr);
		if (_hosting)
		{
			//Check if host should add known connection
			if (receiveResult == 5 && buffer[0] == (unsigned char)MessageType::Join)
			{

				BinaryStream stream;
				for (int i = 0; i < receiveResult; i++)
					stream.Buffer.push_back(buffer[i]);
				stream.Read<byte>();
				int port = stream.Read<int>();

				if (std::find_if(_connections.begin(), _connections.end(), [&ip, &port](const Connection& conn) {return conn.Ip != ip && conn.Port != port; }) != _connections.end())
					continue;
				_lock.lock();
				_connections.push_back({ ip, port });
				printf("Added new connection %s, %d\n", ip.c_str(), port);
				OnConnection(ip, port);
				_lock.unlock();


				sockaddr_in recvAddr;
				recvAddr.sin_family = AF_INET;
				recvAddr.sin_port = htons(port);
				recvAddr.sin_addr.s_addr = inet_addr(ip.c_str());

				byte messageType = (byte)MessageType::Join;
				sendto(_socket, (const char*)& messageType, 1, 0, (SOCKADDR*)& recvAddr, sizeof(recvAddr));
				printf("Sending join response\n");

				continue;
			}
		}
		if (receiveResult == 1 && buffer[0] == (byte)MessageType::Join)
		{
			printf("Received join response\n");
			_connected = true;
			continue;
		}

		BinaryStream stream;

		for (int i = 0; i < receiveResult; i++)
		{
			stream.Buffer.push_back(buffer[i]);
		}
		_lock.lock();
		_streams.push(stream);
		_lock.unlock();
		/*
		while (!stream.EndOfStream()){
			byte typeByte = stream.Read<byte>();
			printf("Type: %d\n", typeByte);
			_messages[(MessageType)typeByte]->Read(&stream);
		}
		*/
	}
	printf("Closing thread");
}

void NetworkClient::SendData(Connection conn)
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

		delete(message);
		_messageQueue.pop();
	}
	if (size == 0)
	{
		return;
	}

	for (auto it : _connections)
	{
		if (conn.Port > 0)
		{
			if (it.Ip != conn.Ip || it.Port != conn.Port)
				continue;
		}

		sockaddr_in recvAddr;
		recvAddr.sin_family = AF_INET;
		recvAddr.sin_port = htons(it.Port);
		recvAddr.sin_addr.s_addr = inet_addr(it.Ip.c_str());

		int result = sendto(_socket, (const char*)&stream.Buffer[0], size, 0, (SOCKADDR*)&recvAddr, sizeof(recvAddr));
		if (result == -1)
		{
			int error = WSAGetLastError();
			printf("Send Error: %d\n", error);
		}
		else
		{
			//printf("Sent %d bytes to %s, with port %d\n", size, it.second.Ip.c_str(), it.second.Port);
		}
	}
}

void NetworkClient::ReadData(NetworkManager& manager)
{
	_lock.lock();

	while (!_streams.empty())
	{
		BinaryStream stream = _streams.front();
		while (!stream.EndOfStream())
		{
			byte typeByte = stream.Read<byte>();
			if (_messages.find((MessageType)typeByte) == _messages.end())
				break;
			_messages[(MessageType)typeByte]->Read(&stream, manager);
		}
		_streams.pop();
	}

	_lock.unlock();
}

void NetworkClient::AddMessageToQueue(Message* message, MessageType type)
{
	_messageQueue.push({type, message});
}

void NetworkClient::RegisterMessage(Message* message, MessageType type)
{
	//TODO: copy and make our own message, to ensure it is not destroyed
	_messages[type] = message;
}

void NetworkClient::Join(std::string ip, int port)
{
	_hosting = false;
	_connections.push_back({ ip, port });


	_joinThread = new std::thread([=] { JoinLoop(ip, port); });
}

void NetworkClient::JoinLoop(std::string ip, int port)
{
	sockaddr_in RecvAddr;
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(port);
	RecvAddr.sin_addr.s_addr = inet_addr(ip.c_str());

	while (!_bound)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	printf("Writing join message: %d\n", _port);
	BinaryStream stream;
	stream.Write<byte>((byte)MessageType::Join);
	stream.Write<int>(_port);

	while (!_connected)
	{
		int result = sendto(_socket, (const char*)& stream.Buffer[0], sizeof(byte) + sizeof(int), 0,
		                    (SOCKADDR*)& RecvAddr, sizeof(RecvAddr));
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		std::printf("Sending join request\n");
	}
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
