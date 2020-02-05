#include "Server.h"
#include "Game/Player.h"
#include "Engine/Engine.h"
#include "Engine/Key.h"
#include "Client.h"
int Bullet::IdCounter = 0;
float Bullet::Speed = 1000;
float Bullet::LifeTime = 3.0f;

#define BULLET_SIZE 15

Server::Server() : SocketClient(50000)
{
	SocketClient.RegisterMessage((Message*)new PlayerMessage(), MessageType::Player);
	SocketClient.RegisterMessage((Message*)new InputMessage(), MessageType::Input);
	SocketClient.RegisterMessage((Message*)new ConnectionIdMessage(), MessageType::ConnectionId);
	SocketClient.RegisterMessage((Message*)new SpawnPlayerMessage(), MessageType::PlayerSpawnMessage);
	SocketClient.RegisterMessage((Message*)new BulletMessage(), MessageType::Bullet);
	SocketClient.RegisterMessage((Message*)new KillMessage(), MessageType::Kill);

	SocketClient.Host();

}

void Server::Update()
{
	const int size = _bullets.size();
	for (int i = size - 1; i >= 0; i--)
	{
		_bullets[i].Position += _bullets[i].Velocity * engDeltaTime();
		_bullets[i].Time += engDeltaTime();

		//Collision
		for (auto& it : _players)
		{
			BoundingBox player = BoundingBox(it.second.Position.X, it.second.Position.Y, it.second.W, it.second.H);
			BoundingBox bullet = BoundingBox(_bullets[i].Position.X, _bullets[i].Position.Y, BULLET_SIZE, BULLET_SIZE);

			if (bullet.CollidesWith(player) && _bullets[i].OwnerId != it.first)
			{
				KillMessage* killMessage = new KillMessage();
				killMessage->BulletId = _bullets[i].Id;
				killMessage->HitPlayerId = it.first;
				killMessage->KillerId = _bullets[i].OwnerId;
				SocketClient.AddMessageToQueue((Message*)killMessage, MessageType::Kill);

				printf("Hit player\n");
				it.second.Position = { 10000, 10000 };
				it.second.Dead = true;
				_bullets[i].Time = Bullet::LifeTime + 1.0f;
				break;
			}
		}
				
		BulletMessage* bulletMessage = new BulletMessage();
		bulletMessage->Id = _bullets[i].Id;
		bulletMessage->Position = _bullets[i].Position;
		bulletMessage->DestroyFlag = _bullets[i].Time > Bullet::LifeTime;
		SocketClient.AddMessageToQueue((Message*)bulletMessage, MessageType::Bullet);

		if (_bullets[i].Time > Bullet::LifeTime)
		{
			_bullets.erase(_bullets.begin() + i);
		}

	}
	for (auto& it : _players)
	{
		it.second.ServerUpdate(*this);

		PlayerMessage* playerMessage = new PlayerMessage();
		playerMessage->Id = it.first;
		playerMessage->X = it.second.Position.X;
		playerMessage->Y = it.second.Position.Y;
		playerMessage->FrameId = _processedFramesPerPlayer[it.first];
		SocketClient.AddMessageToQueue((Message*)playerMessage, MessageType::Player);
	}


	engineUpdate(true);

	//Fake ping
	std::this_thread::sleep_for(std::chrono::milliseconds(30));
	SocketClient.ReadData(*this);
	SocketClient.SendData();
}

void Server::UpdatePlayer(const int id, const int x, const int y, byte buttons, const int frameId, const float deltaTime)
{
	if (_players.find(id) == _players.end())
		return;

	if (_processedFramesPerPlayer[id] >= frameId)
		return;

	//Fake loss percentage
	//if ((std::rand() % 1000) < 20)	//2%
	//	return;

	bool jump = (buttons & (1 << 0)) > 0;
	bool shoot = (buttons & (1 << 1)) > 0;

	//printf("Running player update %d}\n", frameId);

	_players[id].Update(x, y, jump, shoot, world, deltaTime, *this);
	_processedFramesPerPlayer[id] = frameId;
}

void Server::OnConnect(const std::string& ip, int port)
{
	printf("Connected: %s\n", ip.c_str());
	ConnectionIdMessage* message = new ConnectionIdMessage();
	const int id = IdCounter++;
	message->Id = id;

	SocketClient.SendData();
	SocketClient.AddMessageToQueue((Message*)message, MessageType::ConnectionId);
	SocketClient.SendData({ ip, port });

	//_players[id] = Player();
	_players[id].Id = id;
	_players[id].Position = world.SpawnPoints[id % world.SpawnPoints.size()];
	_players[id].W = _players[id].H = 50;
	_processedFramesPerPlayer[id] = 0;
	printf("Player count: %d\n", (int)_players.size());

	for (const auto it : _players)
	{
		SpawnPlayerMessage* playerSpawnMessage = new SpawnPlayerMessage();
		playerSpawnMessage->OwnerId = it.first;
		playerSpawnMessage->X = it.second.Position.X;
		playerSpawnMessage->Y = it.second.Position.Y;
		SocketClient.AddMessageToQueue((Message*)playerSpawnMessage, MessageType::PlayerSpawnMessage);
	}
	SocketClient.SendData();
}

void Server::AddBullet(int id, Vector2 position, int direction)
{
	_bullets.push_back(Bullet{ Bullet::IdCounter++, id, position, Vector2(direction * Bullet::Speed, 0.0f) });
}

//Messages

void ConnectionIdMessage::Read(BinaryStream* stream, NetworkManager& manager)
{
	Id = stream->Read<int>();
	Client& client = (Client&)manager;
	client.Id = Id;
	printf("Received Id from server: %d\n", Id);
}

int ConnectionIdMessage::Write(BinaryStream* stream)
{
	stream->Write<int>(Id);
	return sizeof(int);
}

void SpawnPlayerMessage::Read(BinaryStream* stream, NetworkManager& manager)
{
	OwnerId = stream->Read<int>();
	X = stream->Read<float>();
	Y = stream->Read<float>();

	printf("Received player spawn message: id: %d, x: %f, y: %f\n", OwnerId, X, Y);

	Client& client = (Client&)manager;
	client.AddNewPlayer(OwnerId, X, Y);
}

int SpawnPlayerMessage::Write(BinaryStream* stream)
{
	stream->Write<int>(OwnerId);
	stream->Write<float>(X);
	stream->Write<float>(Y);

	printf("Sending player spawn message\n");
	return sizeof(int) * 3;
}

void BulletMessage::Read(BinaryStream* stream, NetworkManager& manager)
{
	Id = stream->Read<int>();
	Position.X = stream->Read<float>();
	Position.Y = stream->Read<float>();
	DestroyFlag = stream->Read<bool>();

	Client& client = (Client&) manager;
	client.UpdateBullets(Id, Position, DestroyFlag);
}

int BulletMessage::Write(BinaryStream* stream)
{
	stream->Write<int>(Id);
	stream->Write<float>(Position.X);
	stream->Write<float>(Position.Y);
	stream->Write<bool>(DestroyFlag);
	return sizeof(int) * 3 + sizeof(bool);
}

void KillMessage::Read(BinaryStream* stream, NetworkManager& manager)
{
	HitPlayerId = stream->Read<int>();
	KillerId = stream->Read<int>();
	BulletId = stream->Read<int>();

	Client& client = (Client&)manager;
	client.KillPlayer(HitPlayerId, KillerId, BulletId);
}

int KillMessage::Write(BinaryStream* stream)
{
	stream->Write<int>(HitPlayerId);
	stream->Write<int>(KillerId);
	stream->Write<int>(BulletId);

	return sizeof(int) * 3;
}
