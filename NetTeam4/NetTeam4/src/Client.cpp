#include "Client.h"
#include "Game/Player.h"
#include "Server.h"
#include "Engine/Engine.h"
#include "Engine/Key.h"

Client::Client() : SocketClient(60000)
{
	SocketClient.RegisterMessage((Message*)new PlayerMessage(), MessageType::Player);
	SocketClient.RegisterMessage((Message*)new InputMessage(), MessageType::Input);
	SocketClient.RegisterMessage((Message*)new ConnectionIdMessage(), MessageType::ConnectionId);
}

void Client::Join(const std::string& ip, const int port)
{
	SocketClient.Join(ip, port);
}

void Client::Update()
{
	if (_players.find(Id) != _players.end())
	{
		int x = 0, y = 0;
		if (engGetKey(Key::W))
			y -= 1;
		if (engGetKey(Key::S))
			y += 1;
		if (engGetKey(Key::A))
			x -= 1;
		if (engGetKey(Key::D))
			x += 1;

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
