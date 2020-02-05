#include "Client.h"
#include "Game/Player.h"
#include "Server.h"
#include "Engine/Engine.h"
#include "Engine/Key.h"

int InputFrame::FrameCounter = 0;
float ClientBullet::Speed = 30;

#define LERP_SPEED 35.0f

Client::Client() : SocketClient(60000)
{
	//Register all Client messages
	SocketClient.RegisterMessage((Message*)new PlayerMessage(), MessageType::Player);
	SocketClient.RegisterMessage((Message*)new InputMessage(), MessageType::Input);
	SocketClient.RegisterMessage((Message*)new ConnectionIdMessage(), MessageType::ConnectionId);
	SocketClient.RegisterMessage((Message*)new SpawnPlayerMessage(), MessageType::PlayerSpawnMessage);
	SocketClient.RegisterMessage((Message*)new BulletMessage(), MessageType::Bullet);
	SocketClient.RegisterMessage((Message*)new KillMessage(), MessageType::Kill);

}

void Client::Join(const std::string& ip, const int port)
{
	SocketClient.Join(ip, port);
}

void Client::Update()
{
	//Update engine
	engineUpdate(false);
	if (engGetKeyDown(Key::Escape))
		engClose();

	//Update local player
	if (_players.find(Id) != _players.end())
	{
		//Local player update

		int x = 0, y = 0;
		if (engGetKey(Key::W))
			y -= 1;
		if (engGetKey(Key::S))
			y += 1;
		if (engGetKey(Key::A))
			x -= 1;
		if (engGetKey(Key::D))
			x += 1;

		bool jump = engGetKeyDown(Key::Space);
		bool shoot = engGetKeyDown(Key::O);

		byte buttons = 0;
		if(jump)
			buttons |= 1 << 0;
		if(shoot)
			buttons |= 1 << 1;

		_players[Id].Update(x, y, jump, false, world, engDeltaTime(), *this);

		InputMessage* message = new InputMessage(Id, x, y, buttons, InputFrame::FrameCounter, engDeltaTime());
		SocketClient.AddMessageToQueue((Message*)(message), MessageType::Input);
		//printf("Client: %f %f \n", _players[Id].Position.X, _players[Id].Position.Y);

		//Error correction
		_players[Id].Position += Vector2(ErrorX, ErrorY) * 0.2f;
		ErrorX *= 0.8f;
		ErrorY *= 0.8f;

		_frames.push(InputFrame{ _players[Id].Position.X, _players[Id].Position.Y, ErrorX, ErrorY, InputFrame::FrameCounter });
		InputFrame::FrameCounter++;
	}
	//TODO: update slave players, non local players

	//Draw all players
	for (auto& it : _players)
	{
		if (it.first != Id)
		{
			float prevX = it.second.Position.X;
			it.second.Position = mathHelper::lerp(it.second.Position, _serverPositions[it.first], mathHelper::clamp(LERP_SPEED * engDeltaTime(), 0.0f, 1.0f));
			int dir = mathHelper::sign(it.second.Position.X - prevX);
			it.second.FacingDirection = dir;
		}
		
		engDrawRect((int)std::round(it.second.Position.X), (int)std::round(it.second.Position.Y), it.second.W, it.second.H);
	}
	for (auto& it : _bullets)
	{
		it.second.Position = mathHelper::lerp(it.second.Position, it.second.TargetPosition, ClientBullet::Speed * engDeltaTime());
		engDrawRect((int)std::round(it.second.Position.X), (int)std::round(it.second.Position.Y), it.second.W, it.second.H);
	}
	//TODO: draw world and props
	world.Update();


	//Update network client
	SocketClient.SendData();
	SocketClient.ReadData(*this);
}

//Server messages

void Client::AddNewPlayer(const int ownerId, const float x, const float y)
{
	printf("Received player message with id: %d\n", ownerId);
	if (_players.find(ownerId) != _players.end())
		return;

	//_players[ownerId] = Player();
	_players[ownerId].Id = ownerId;
	_players[ownerId].Position.X = x;
	_players[ownerId].Position.Y = y;
	_players[ownerId].W = _players[ownerId].H = 50;
	_serverPositions[ownerId] = { x,y };
}

void Client::UpdatePlayer(const int ownerId, const float x, const float y, const int frameId)
{
	//Check if player has spawned yet
	if (_players.find(ownerId) == _players.end())
		return;

	//Slave player
	if (ownerId != Id)
	{
		_serverPositions[ownerId] = { x,y };
		
		float delta = (_players[ownerId].Position - Vector2{ x,y }).magnitude();
		if (delta > 200)
			_players[ownerId].Position = { x,y };

		return;
	}

	//Local player error correction
	while (!_frames.empty())
	{
		if (_frames.front().FrameId >= frameId)
			break;
		_frames.pop();
	}

	if (_frames.empty())
	{
		return;
	}

	const InputFrame frame = _frames.front();
	ErrorX = (x - frame.X) - frame.RemainingX;
	ErrorY = (y - frame.Y) - frame.RemainingY;
	//printf("Server: %f, %f, Error: %f, %f\n", x, y, ErrorX, ErrorY);
	if ((_players[Id].Position - Vector2{ ErrorX, ErrorY }).magnitude() > 200)
	{
		_players[Id].Position = { x,y };
		ErrorX = ErrorY = 0.0f;
	}
	
	//Update all frames since server present to show that remaining error to correct is the full error
	std::vector<InputFrame> vec;
	while (!_frames.empty())
	{
		InputFrame f = _frames.front();
		_frames.pop();
		f.RemainingX = ErrorX;
		f.RemainingY = ErrorY;
		vec.push_back(f);
	}
	for (auto it : vec)
		_frames.push(it);

	//TODO: should probably make frames a vector instead to prevent this ugly popping and re-pushing
}

void Client::UpdateBullets(int id, Vector2 position, bool destroyFlag)
{
	if (_bullets.find(id) == _bullets.end())
	{
		_bullets[id].Id = id;
		_bullets[id].Position = _bullets[id].TargetPosition = position;
	}
	_bullets[id].TargetPosition = position;
	if (destroyFlag)
		_bullets.erase(id);
}

void Client::KillPlayer(const int playerId, const int killerId, const int bulletId)
{
	if (_players.find(playerId) == _players.end())
		return;

	_players[playerId].Position = { 10000, 10000 };
}
