#pragma once
#include <thread>
#include <WinSock2.h>
#include "Message.h"
#include <queue>
#include <tuple>
#include <unordered_map>
#include <mutex>
#include <functional>
#include "../NetworkManager.h"

enum class MessageType : unsigned char
{
	   Test = 0,
	   OtherTest,
	   Player,
	   Join,
	   Input,
	   ConnectionId,
	   PlayerSpawnMessage,
};
struct Connection
{
	std::string Ip;
	int Port;
};


class NetworkClient
{
public:
	NetworkClient() = default;
	NetworkClient(int port);
	~NetworkClient();
	void Close();
	void SendData(std::string ip = "");
	void ReadData(NetworkManager& manager);
	void AddMessageToQueue(Message* message, MessageType type);
	void RegisterMessage(Message* message, MessageType type);
	void Join(std::string ip, int port);
	void JoinLoop(std::string ip, int port);
	void Host();

	std::function<void(std::string)> OnConnection;

private:
	void Listen();
	
	std::thread _thread;
	std::thread* _joinThread;
	SOCKET _socket;
	std::queue<std::tuple<MessageType, Message*>> _messageQueue;
	bool _bound = false;
	int _port;
	bool _closeThread = false;
	bool _hosting = false;
	bool _connected = false;
	std::queue<BinaryStream> _streams;
	std::mutex _lock;
	std::unordered_map<MessageType, Message*> _messages;
	std::unordered_map<std::string, Connection> _connections;
};

