#pragma once

struct BoundingBox
{


	int X;
	int Y;
	int Width;
	int Height;

	BoundingBox() : X(0), Y(0), Width(0), Height(0)	{	}

	BoundingBox(int X, int Y, unsigned int Width, unsigned int Height) : X(X), Y(Y), Width(Width), Height(Height)	{	}

	// Returns if given point lies inside of the bounding box
	bool ContainsPoint(int PointX, int PointY) const
	{
		return PointX >= X && PointX < X + Width &&
			PointY >= Y && PointY < Y + Height;
	}

	// Returns if this bounding box intersects the other
	bool CollidesWith(const BoundingBox& Other) const
	{
		return Other.X < X + Width && Other.X + Other.Width > X &&
			Other.Y < Y + Height && Other.Y + Other.Height > Y;
	}

	// Returns if the other bounding box is fully contained within this bounding box
	bool Contains(const BoundingBox& Other) const
	{
		return Other.X >= X && Other.X + Other.Width <= X + Width &&
			Other.Y >= Y && Other.Y + Other.Height <= Y + Height;
	}
};