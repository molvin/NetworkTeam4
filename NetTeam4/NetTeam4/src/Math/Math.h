#pragma once
#include <cmath>

struct Vector2
{
	float X;
	float Y;

	Vector2() : X(0.0f), Y(0.0f) { }
	Vector2(float x, float y) : X(x), Y(y) { }
	Vector2(float value) : X(value), Y(value) { }

	Vector2 operator+(Vector2 otherVec) const
	{
		return Vector2(X + otherVec.X, Y + otherVec.Y);
	}
	Vector2 operator-(Vector2 otherVec) const
	{
		return Vector2(X - otherVec.X, Y - otherVec.Y);
	}
	Vector2 operator*(float scalar) const
	{
		return Vector2(X * scalar, Y * scalar);
	}
	Vector2 operator/(float scalar) const
	{
		return Vector2(X / scalar, Y / scalar);
	}
	Vector2& operator+=(const Vector2& otherVec)
	{
		*this = *this + otherVec;
		return *this;
	}
	Vector2& operator-=(const Vector2& otherVec)
	{
		*this = *this - otherVec;
		return *this;
	}
	Vector2& operator*=(float scalar)
	{
		*this = *this * scalar;
		return *this;
	}
	Vector2& operator/=(float scalar)
	{
		*this = *this / scalar;
		return *this;
	}

	float magnitude()
	{
		return std::sqrt(X * X + Y * Y);
	}
	void Normalize()
	{
		Vector2 vec = *this / magnitude();
		X = vec.X;
		Y = vec.Y;
	}
	Vector2 Normalized()
	{
		return *this / magnitude();
	}
	static float Dot(const Vector2& lhs, const Vector2& rhs)
	{
		return lhs.X * lhs.X + lhs.Y * rhs.Y;
	}


	static Vector2 Zero;
	static Vector2 Right;
	static Vector2 Up;
	static Vector2 One;

};