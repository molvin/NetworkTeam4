#include "Player.h"
#include "../Client.h"

void PlayerMessage::Read(BinaryStream* stream, NetworkManager& manager){

	Id = stream->Read<int>();
	x = stream->Read<int>();
	y = stream->Read<int>();

	Client& client = (Client&)manager;
	client.UpdatePlayer(Id, x, y);
}

int PlayerMessage::Write(BinaryStream* stream){

	stream->Write<int>(Id);
	stream->Write<int>(x);
	stream->Write<int>(y);

	return sizeof(int) * 3;
}
