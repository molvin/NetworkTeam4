#pragma once
#include "../Networking/Message.h"
#include "../Math/Math.h"
#include "../Game/World.h"

class Player
{
public:
	int Id;
	Vector2 Position;				//TODO: update to vectors, position, size
	int  W, H = 50;

	void Update(const int inputX, const int inputY, const World& world);
};

class PlayerMessage : Message
{
public:
	int Id;
	float X;
	float Y;
	int FrameId;

	virtual void Read(BinaryStream* stream, NetworkManager& manager) override;
	virtual int Write(BinaryStream* stream) override;
};

class InputMessage : Message
{
public:
	int Id, X, Y, FrameId;

	virtual void Read(BinaryStream* stream, NetworkManager& manager) override;
	virtual int Write(BinaryStream* stream) override;
	InputMessage() = default;
	InputMessage(const int id, const int x, const int y, const int frameId) : Id(id), X(x), Y(y), FrameId(frameId){}
};


