#pragma once
#include "../Networking/Message.h"

class Player
{
public:
	int Id;
	float x, y, w, h;
	int LastProcessedServerFrame;

};

class PlayerMessage : Message
{
public:
	int Id;
	float x;
	float y;
	int frameId;

	virtual void Read(BinaryStream* stream, NetworkManager& manager) override;
	virtual int Write(BinaryStream* stream) override;
};

