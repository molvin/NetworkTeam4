#include "Client.h"
#include "Game/Player.h"
#include "Server.h"
#include "Engine/Engine.h"
#include "Engine/Key.h"

int InputFrame::FrameCounter = 0;

Client::Client() : SocketClient(60000)
{
	//Register all Client messages
	SocketClient.RegisterMessage((Message*)new PlayerMessage(), MessageType::Player);
	SocketClient.RegisterMessage((Message*)new InputMessage(), MessageType::Input);
	SocketClient.RegisterMessage((Message*)new ConnectionIdMessage(), MessageType::ConnectionId);
	SocketClient.RegisterMessage((Message*)new SpawnPlayerMessage(), MessageType::PlayerSpawnMessage);
}

void Client::Join(const std::string& ip, const int port)
{
	SocketClient.Join(ip, port);
}

void Client::Update()
{
	//Update engine
	engineUpdate();
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

		_players[Id].Update(x, y);

		InputMessage* message = new InputMessage(Id, x, y, InputFrame::FrameCounter);
		SocketClient.AddMessageToQueue((Message*)(message), MessageType::Input);

		//Error correction
		_players[Id].X += ErrorX * 0.2f;
		ErrorX *= 0.8f;
		_players[Id].Y += ErrorY * 0.2f;
		ErrorY *= 0.8f;

		_frames.push(InputFrame{ _players[Id].X, _players[Id].Y, ErrorX, ErrorY, InputFrame::FrameCounter });
		InputFrame::FrameCounter++;
	}
	//TODO: update slave players, non local players

	//Draw all players
	for (const auto it : _players)
	{
		engDrawRect((int)std::round(it.second.X), (int)std::round(it.second.Y), it.second.W, it.second.H);
	}
	//TODO: draw world and props


	//Update network client
	SocketClient.SendData();
	SocketClient.ReadData(*this);
}

//Server messages

void Client::AddNewPlayer(const int ownerId, const float x, const float y)
{
	if (_players.find(ownerId) != _players.end())
		return;

	_players[ownerId] = Player();
	_players[ownerId].Id = ownerId;
	_players[ownerId].X = x;
	_players[ownerId].Y = y;
	_players[ownerId].W = _players[ownerId].H = 50;
}

void Client::UpdatePlayer(const int ownerId, const float x, const float y, const int frameId)
{
	//Check if player has spawned yet
	if (_players.find(ownerId) == _players.end())
		return;

	//Slave player
	if (ownerId != Id)
	{
		_players[ownerId].X = x;
		_players[ownerId].Y = y;
		//TODO: interpolation to prevent stuttering
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