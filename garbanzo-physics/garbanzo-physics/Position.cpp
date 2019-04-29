#include "Position.h"


Position::Position(float xPos, float yPos)
{
	x = xPos;
	y = yPos;
}

Position::Position()
{
	x = 0.f;
	y = 0.f;
}

Position::~Position()
{

}

void Position::SetX(float newX)
{
	x = newX;
}
void Position::SetY(float newY)
{
	y = newY;
}
