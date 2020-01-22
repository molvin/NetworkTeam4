#pragma once
#include "../Networking/Message.h"

class Player
{
public:
	int Id;
	int x, y, w, h;

};

class PlayerMessage : Message
{
public:
	int Id;
	int x;
	int y;
	int frameId;

	virtual void Read(BinaryStream* stream, NetworkManager& manager) override;
	virtual int Write(BinaryStream* stream) override;
};

