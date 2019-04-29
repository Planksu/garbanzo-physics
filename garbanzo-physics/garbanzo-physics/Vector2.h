#ifndef _VECTOR2_HEADER_
#define _VECTOR2_HEADER_


class Vector2
{
private:
	float x;
	float y;

public:
	Vector2(float xPos, float yPos);
	Vector2();
	~Vector2();

	float GetX() const { return x; }
	float GetY() const { return y; }

	void SetX(float newX);
	void SetY(float newY);

	Vector2 operator-(const Vector2 &other) const
	{
		return Vector2(this->GetX() - other.GetX(), this->GetY() - other.GetY());
	}
};

#endif
