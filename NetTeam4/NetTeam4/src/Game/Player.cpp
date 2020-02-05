#include "Player.h"
#include "../Client.h"
#include "../Server.h"
#include "../Game/World.h"
#include "../Engine/Engine.h"
#include "../Math/Math.h"

//TODO: proper movement
//TODO: super mario collision

#define DEATH_DURATION 1.5f

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
	DeltaTime = stream->Read<float>();
	Buttons = stream->Read<byte>();

	//Reading on the server
	Server& server = (Server&)manager;
	server.UpdatePlayer(Id, X, Y, Buttons, FrameId, DeltaTime);
}

int InputMessage::Write(BinaryStream* stream)
{
	stream->Write<int>(Id);
	stream->Write<int>(X);
	stream->Write<int>(Y);
	stream->Write<int>(FrameId);
	stream->Write<float>(DeltaTime);
	stream->Write<byte>(Buttons);
	return sizeof(int) * 5 + 1;
}
void Player::Update(const int inputX, const int inputY, bool jump, bool shoot, const World& world, const float deltaTime, NetworkManager& manager)
{

	Velocity -= Vector2::Up * Gravity * deltaTime;
	
	if(mathHelper::abs(Velocity.X + inputX * Acceleration * deltaTime) < MaxSpeed)
		Velocity += Vector2(inputX, 0.0f) * Acceleration * deltaTime;

	if (inputX != 0)
		FacingDirection = inputX;
	
	//TODO: do something with y input, i.e fast falling

	Vector2 normal;
	if (world.Colliding(BoundingBox(Position.X + Velocity.X * deltaTime, Position.Y + Velocity.Y * deltaTime, W, H), normal))
	{
		if (Vector2::Dot(normal, Velocity) < 0.0f)
		{
			Velocity -= normal * Vector2::Dot(normal, Velocity);
			Grounded = true;
			//printf("Collided: Velo: %f, %f\n", Velocity.X, Velocity.Y);
		}

	}
	else
	{
		Grounded = false;
	}

	if (Grounded && (inputX == 0 || Vector2::Dot(Velocity, Vector2(inputX, 0.0f)) < 0.0f))
	{
		Velocity = mathHelper::lerp(Velocity, Vector2::Zero, Friction * deltaTime);
	}

	if (jump && Grounded)
	{
		Velocity.Y = -JumpSpeed;
		Grounded = false;
	}
	   
	Position += Velocity * deltaTime;

	//Server only shooting

	if (shoot)
	{
		Server& server = (Server&)manager;
		server.AddBullet(Id, Position, FacingDirection);
	}


	

}

void Player::ServerUpdate(Server& server)
{
	if (Dead)
	{
		DeathTimer += engDeltaTime();
		if (DeathTimer > DEATH_DURATION)
		{
			DeathTimer = 0.0f;
			Dead = false;
			Position = server.world.SpawnPoints[Id % server.world.SpawnPoints.size()];
			Velocity = Vector2::Zero;
		}
	}
}
