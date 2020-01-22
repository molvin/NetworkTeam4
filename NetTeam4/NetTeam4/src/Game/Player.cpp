#include "Player.h"
#include "../Client.h"

void PlayerMessage::Read(BinaryStream* stream, NetworkManager& manager)
{
	Id = stream->Read<int>();
	x = stream->Read<float>();
	y = stream->Read<float>();
	frameId = stream->Read<int>();

	Client& client = (Client&)manager;
	client.UpdatePlayer(Id, x, y, frameId);
}

int PlayerMessage::Write(BinaryStream* stream){

	stream->Write<int>(Id);
	stream->Write<float>(x);
	stream->Write<float>(y);
	stream->Write<int>(frameId);

	return sizeof(int) * 4;
}
