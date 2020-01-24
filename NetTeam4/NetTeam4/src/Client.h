#pragma once
#include "Networking/NetworkClient.h"
#include "Game/Player.h"
#include <map>
#include "Engine/Collision.h"
#include "Game/World.h"

struct InputFrame
{
	float X, Y;
	float RemainingX, RemainingY;
	int FrameId;
	static int FrameCounter;
};

class Client : NetworkManager
{
public:
	int Id = -1;
	Client();
	void Join(const std::string& ip, const int port);
	void Update();
	void AddNewPlayer(int ownerId, float x, float y);
	void UpdatePlayer(int ownerId, float x, float y, int frameId);

	NetworkClient SocketClient;
	std::map<int, Player> _players;
	std::queue<InputFrame> _frames;
	float ErrorX = 0.0f;
	float ErrorY = 0.0f;
	World world;

};

