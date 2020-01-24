#pragma once
#include <cmath>

struct Vector2
{
	float X;
	float Y;

	Vector2() : X(0.0f), Y(0.0f) { }
	Vector2(float x, float y) : X(x), Y(y) { }
	Vector2(float value) : X(value), Y(value) { }
	Vector2(const Vector2& other) : X(other.X), Y(other.Y) { }

	Vector2 operator +(const Vector2& other)
	{
		return Vector2(X + other.X, Y + other.Y);
	}
	Vector2& operator +=(const Vector2& other)
	{
		X += other.X;
		Y += other.Y;
		return *this;
	}
	Vector2 operator -(const Vector2& other)
	{
		return Vector2(X - other.X, Y - other.Y);

	}
	Vector2 operator =(const Vector2& other)
	{
		return Vector2(other.X, other.Y);
	}
	Vector2 operator *(const float value)
	{
		return Vector2(X * value, Y * value);

	}
	Vector2 operator /(const float value)
	{
		return Vector2(X / value, Y / value);
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