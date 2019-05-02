#ifndef _VECTOR2_HEADER_
#define _VECTOR2_HEADER_


class Vector2
{
public:
	float x;
	float y;

	Vector2(float xPos, float yPos);
	Vector2();
	~Vector2();

	void SetX(float newX);
	void SetY(float newY);

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
};

#endif
