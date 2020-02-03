#pragma once
#include <vector>
#include "../Math/Math.h"

class BoundingBox;

class World
{
public:
	World();
	void Update() const;
	void Draw() const;
	bool Colliding(const BoundingBox& collider, Vector2& normal) const;

private:
	std::vector<BoundingBox> Colliders;
};

