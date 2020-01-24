#include "Player.h"
#include "../Client.h"
#include "../Server.h"
#include "../Game/World.h"

//TODO: proper movement
//TODO: super mario collision

void PlayerMessage::Read(BinaryStream* stream, NetworkManager& manager)
{
	Id = stream->Read<int>();
	X = stream->Read<float>();
	Y = stream->Read<float>();
	FrameId = stream->Read<int>();

	Client& client = (Client&)manager;
	client.UpdatePlayer(Id, X, Y, FrameId);
}

int PlayerMessage::Write(BinaryStream* stream){

	stream->Write<int>(Id);
	stream->Write<float>(X);
	stream->Write<float>(Y);
	stream->Write<int>(FrameId);

	return sizeof(int) * 4;
}

void InputMessage::Read(BinaryStream* stream, NetworkManager& manager)
{
	Id = stream->Read<int>();
	X = stream->Read<int>();
	Y = stream->Read<int>();
	FrameId = stream->Read<int>();

	//Reading on the server
	Server& server = (Server&)manager;
	server.UpdatePlayer(Id, X, Y, FrameId);
}

int InputMessage::Write(BinaryStream* stream)
{
	stream->Write<int>(Id);
	stream->Write<int>(X);
	stream->Write<int>(Y);
	stream->Write<int>(FrameId);
	return sizeof(int) * 4;
}

void Player::Update(const int inputX, const int inputY, const World& world)
{
	if (!world.Colliding(BoundingBox(Position.X + inputX, Position.Y + inputY, W, H)))
		Position += Vector2(inputX, inputY);

}
