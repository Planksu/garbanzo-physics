#ifndef _VECTOR2_HEADER_
#define _VECTOR2_HEADER_


class Vector2
{
public:
	float x;
	float y;

	Vector2(float xPos, float yPos);
	Vector2();
	float magnitude();
	~Vector2();

	Vector2 operator-(const Vector2 &other) const
	{
		return Vector2(this->x - other.x, this->y - other.y);
	}

	Vector2 operator+(const Vector2 &other) const
	{
		return Vector2(this->x + other.x, this->y + other.y);
	}

	Vector2 operator*(const Vector2 &other) const
	{
		return Vector2(this->x * other.x, this->y * other.y);
	}

	Vector2 operator*(const float &other) const
	{
		return Vector2(this->x * other, this->y * other);
	}
	Vector2 operator/(const float &other) const
	{
		return Vector2(this->x / other, this->y / other);
	}
	Vector2 getNormal()
	{
		return Vector2(-this->y, this->x);
	}
	
};

#endif
