#pragma once
#include "Networking/NetworkClient.h"
#include "Game/Player.h"
#include "Game/World.h"
#include "NetworkManager.h"

struct Bullet
{
	static int IdCounter;
	int Id;
	int OwnerId;
	Vector2 Position;
	Vector2 Velocity;
	static float Speed;
};


class Server : NetworkManager
{
public:
	int IdCounter = 0;
	Server();
	void Update();
	void UpdatePlayer(int id, int x, int y, byte buttons, int frameId, const float deltaTime);
	void OnConnect(const std::string& ip, int port);
	void AddBullet(int id, Vector2 position, int direction);

	NetworkClient SocketClient;
	std::unordered_map<int, Player> _players;
	std::unordered_map<int, int> _processedFramesPerPlayer;
	World world;
	std::vector<Bullet> _bullets;
};

class ConnectionIdMessage : Message
{
public:
	int Id;

	virtual void Read(BinaryStream* stream, NetworkManager& manager) override;

	virtual int Write(BinaryStream* stream) override;

};

class SpawnPlayerMessage : Message
{
public:
	int OwnerId;
	float X, Y;

	virtual void Read(BinaryStream* stream, NetworkManager& manager) override;

	virtual int Write(BinaryStream* stream) override;
};

class BulletMessage : Message
{
public:
	int Id;
	Vector2 Position;

	virtual void Read(BinaryStream* stream, NetworkManager& manager) override;
	virtual int Write(BinaryStream* stream) override;
};
