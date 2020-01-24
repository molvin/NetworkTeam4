#include "World.h"
#include "../Engine/Collision.h"
#include "../Engine/Engine.h"

World::World()
{
	Colliders.push_back(BoundingBox(100, 500, 600, 25));

}

void World::Update()
{
	Draw();
}

void World::Draw() const
{
	for(BoundingBox obj : Colliders)
	{
		engDrawRect(obj.X, obj.Y, obj.Width, obj.Height);
	}
}

bool World::Colliding(const BoundingBox& collider) const
{
	for (BoundingBox obj : Colliders)
	{
		if (collider.CollidesWith(obj))
			return true;
	}
	return false;
}