#include "Client.h"
#include "Game/Player.h"
#include "Server.h"
#include "Engine/Engine.h"
#include "Engine/Key.h"

int InputFrame::frameCounter = 0;

Client::Client() : SocketClient(60000)
{
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
	engineUpdate();

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
		
		//Temp player movement
		_players[Id].x += x;
		_players[Id].y += y;
		_frames.push(InputFrame{ _players[Id].x, _players[Id].y, InputFrame::frameCounter++ });
		//Error correction
		int correction = (int)std::ceil((float)error_x * 0.6f);
		error_x -= correction;
		if (error_x < 0)
			error_x = 0;
		_players[Id].x += correction;
		correction = (int)std::ceil((float)error_y * 0.6f);
		error_y -= correction;
		if (error_y < 0)
			error_y = 0;
		_players[Id].y += correction;
				
		InputMessage* message = new InputMessage();
		message->id = Id;
		message->x = x;
		message->y = y;
		SocketClient.AddMessageToQueue((Message*)message, MessageType::Input);
	}

	for (auto it : _players)
	{
		engDrawRect(it.second.x, it.second.y, it.second.w, it.second.h);
	}


	if (engGetKeyDown(Key::Escape))
		engClose();


	SocketClient.SendData();
	SocketClient.ReadData(*this);
}

void Client::AddNewPlayer(int ownerId, int x, int y)
{
	if (_players.find(ownerId) != _players.end())
	{
		return;
	}

	_players[ownerId] = Player();
	_players[ownerId].Id = ownerId;
	_players[ownerId].x = x;
	_players[ownerId].y = y;
	_players[ownerId].w = _players[ownerId].h = 50;
}

void Client::UpdatePlayer(int ownerId, int x, int y, int frameId)
{
	if (_players.find(ownerId) == _players.end())
	{
		return;
	}

	while (!_frames.empty())
	{
		if (_frames.front().frameId < frameId)
			_frames.pop();
		else
			break;
	}

	InputFrame frame = _frames.front();
	error_x = frame.x - x;
	error_y = frame.y - y;
	//_players[ownerId].x = x;
	//_players[ownerId].y = y;
}

void InputMessage::Read(BinaryStream* stream, NetworkManager& manager)
{
	id = stream->Read<int>();
	x = stream->Read<int>();
	y = stream->Read<int>();

	//Reading on the server
	Server& server = (Server&)manager;
	server.UpdatePlayer(id, x, y);	
}

int InputMessage::Write(BinaryStream* stream)
{
	stream->Write<int>(id);
	stream->Write<int>(x);
	stream->Write<int>(y);
	return sizeof(int) * 3;
}
