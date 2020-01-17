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

void Client::Update(Player& player)
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

	if (engGetKeyDown(Key::Escape))
		engClose();

	InputMessage* message = new InputMessage();
	message->x = x;
	message->y = y;
	SocketClient.AddMessageToQueue((Message*)message, MessageType::Input);

	SocketClient.SendData();
	SocketClient.ReadData(*this);
}

void InputMessage::Read(BinaryStream* stream, NetworkManager& manager)
{
	x = stream->Read<int>();
	y = stream->Read<int>();

	//Reading on the server
	Server& server = (Server&)manager;
	server.UpdatePlayer(x, y);	
}

int InputMessage::Write(BinaryStream* stream)
{
	stream->Write(x);
	stream->Write(y);
	return sizeof(int) * 2;
}
