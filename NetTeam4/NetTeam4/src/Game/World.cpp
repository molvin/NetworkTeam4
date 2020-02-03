#include "World.h"
#include "../Engine/Collision.h"
#include "../Engine/Engine.h"

World::World()
{
	Colliders.emplace_back(0, 575, 10000, 25);
	Colliders.emplace_back(0, 0, 10000, 25);
	Colliders.emplace_back(0, 0, 25, 1000);
	Colliders.emplace_back(775, 0, 25, 1000);

}

void World::Update() const
{
	Draw();
}

void World::Draw() const
{
	for(const BoundingBox& obj : Colliders)
	{
		engDrawRect(obj.X, obj.Y, obj.Width, obj.Height);
	}
}

bool World::Colliding(const BoundingBox& collider, Vector2& normal) const
{
	//TODO: fix more correct normals
	for (BoundingBox obj : Colliders)
	{
		if (collider.CollidesWith(obj))
		{
			//Check which side of the player collided, return the negative normal of that side
			int x, y;
			normal = Vector2(0.0f, -1.0f);
			if (obj.ContainsPoint(collider.X, collider.Y) && obj.ContainsPoint(collider.X + collider.Width, collider.Y))
			{
				//Top side
				normal = Vector2(0.0f, 1.0f);
			}
			else if (obj.ContainsPoint(collider.X, collider.Y + collider.Height) && obj.ContainsPoint(collider.X + collider.Width, collider.Y + collider.Height))
			{
				//Bottom
				normal = Vector2(0.0f, -1.0f);

			}
			else if (obj.ContainsPoint(collider.X + collider.Width, collider.Y) && obj.ContainsPoint(collider.X + collider.Width, collider.Y + collider.Height))
			{
				//Right
				normal = Vector2(-1.0f, 0.0f);
			}
			else if (obj.ContainsPoint(collider.X, collider.Y) && obj.ContainsPoint(collider.X, collider.Y + collider.Height))
			{
				//Left
				normal = Vector2(1.0f, 0.0f);
			}


			return true;
		}
	}
	return false;
}