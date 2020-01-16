#include "Player.h"
Player* PlayerMessage::player = nullptr;

void PlayerMessage::Read(BinaryStream* stream){

	x = stream->Read<int>();
	y = stream->Read<int>();
	player->x = x;
	player->y = y;

}

int PlayerMessage::Write(BinaryStream* stream){

	stream->Write<int>(x);
	stream->Write<int>(y);

	return sizeof(int) * 2;
}
