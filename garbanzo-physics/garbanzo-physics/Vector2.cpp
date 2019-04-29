#include "Vector2.h"


Vector2::Vector2(float xPos, float yPos)
{
	x = xPos;
	y = yPos;
}

Vector2::Vector2()
{
	x = 0.f;
	y = 0.f;
}

Vector2::~Vector2()
{

}

void Vector2::SetX(float newX)
{
	x = newX;
}
void Vector2::SetY(float newY)
{
	y = newY;
}
