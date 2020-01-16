#pragma once
#include <thread>
#include <WinSock2.h>
#include "Message.h"
#include <queue>
#include <tuple>
#include <unordered_map>

enum class MessageType : unsigned char
{
	   Test = 0,
	   OtherTest,
	   Player
};

class NetworkClient
{
public:
	NetworkClient(int port);
	~NetworkClient();
	void Close();
	void SendData();
	void AddMessageToQueue(Message* message, MessageType type);
	void RegisterMessage(Message* message, MessageType type);

private:
	void Listen();
	
	std::thread _thread;
	SOCKET _socket;
	std::queue<std::tuple<MessageType, Message*>> _messageQueue;
	bool _bound = false;
	int _port;
	bool _closeThread = false;
	std::unordered_map<MessageType, Message*> _messages;
};

