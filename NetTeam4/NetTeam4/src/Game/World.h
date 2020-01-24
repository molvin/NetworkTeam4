#pragma once
#include <vector>

class BoundingBox;

class World
{
public:
	World();
	void Update();
	void Draw() const;
	bool Colliding(const BoundingBox& collider) const;

private:
	std::vector<BoundingBox> Colliders;
};

