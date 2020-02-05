#pragma once
#include "Networking/NetworkClient.h"
#include "Game/Player.h"
#include <map>
#include "Engine/Collision.h"
#include "Game/World.h"

struct ClientBullet
{
	int Id;
	Vector2 Position;
	Vector2 TargetPosition;
	int W = 15;
	int H = 15;
	static float Speed;
};

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
	void UpdateBullets(const int id, const Vector2 position, const bool destroyFlag);
	void KillPlayer(const int playerId, const int killerId, const int bulletId);

	NetworkClient SocketClient;
	std::map<int, Player> _players;
	std::queue<InputFrame> _frames;
	std::unordered_map<int, ClientBullet> _bullets;
	std::unordered_map<int, Vector2> _serverPositions;
	float ErrorX = 0.0f;
	float ErrorY = 0.0f;
	World world;

};

