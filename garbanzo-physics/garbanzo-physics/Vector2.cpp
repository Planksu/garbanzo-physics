#include "Vector2.h"
#include <cmath>


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

float Vector2::magnitude()
{
	return sqrt(this->x * this->x + this->y*this->y);
}
