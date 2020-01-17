#pragma once
#include "../Networking/Message.h"

class Player
{
public:
	int x, y, w, h;

};

class PlayerMessage : Message
{
public:
	int x;
	int y;

	virtual void Read(BinaryStream* stream, NetworkManager& manager) override;
	virtual int Write(BinaryStream* stream) override;
	static Player* player;
};

