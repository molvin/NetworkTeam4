#pragma once
#include "Networking/NetworkClient.h"
#include "Game/Player.h"
#include <map>

struct InputFrame
{
	int x;
	int y;
	int frameId;
	static int frameCounter;
};

class InputMessage : Message
{
public:
	int id, x, y;

	virtual void Read(BinaryStream* stream, NetworkManager& manager) override;
	virtual int Write(BinaryStream* stream) override;
};

class Client : NetworkManager
{
public:
	int Id = -1;
	Client();
	void Join(const std::string& ip, const int port);
	void Update();
	void AddNewPlayer(int ownerId, int x, int y);
	void UpdatePlayer(int ownerId, int x, int y, int frameId);

	NetworkClient SocketClient;
	std::map<int, Player> _players;
	std::queue<InputFrame> _frames;
	int error_x;
	int error_y;
};

