#pragma once
#include "../Networking/Message.h"
#include "../Math/Math.h"
#include "../Game/World.h"
#include "../Math/Math.h"

class Player
{
public:
	int Id;
	Vector2 Position;				
	int  W, H = 50;

	Vector2 Velocity;
	const float Gravity = 1200.0f;
	const float Acceleration = 850.0f;
	const float Friction = 15.0f;
	const float MaxSpeed = 200.0f;
	const float JumpSpeed = 500.0f;
	bool Grounded;

	Player() = default;
	void Update(const int inputX, const int inputY, const bool jump, const bool shoot, const World& world, const float deltaTime, NetworkManager& manager);
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
	float DeltaTime;
	byte Buttons;

	virtual void Read(BinaryStream* stream, NetworkManager& manager) override;
	virtual int Write(BinaryStream* stream) override;
	InputMessage() = default;
	InputMessage(const int id, const int x, const int y, byte buttons, const int frameId, const float deltaTime) : Id(id), X(x), Y(y), Buttons(buttons), FrameId(frameId), DeltaTime(deltaTime){}
};


