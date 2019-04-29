#include "Object.h"



Object::Object(SDL_Rect rect, Position p, RGB col, float m)
{
	r = rect;
	pos = p;
	color = col;
	mass = m;

	SetPos(p.GetX(), p.GetY());
}


Object::~Object()
{
}

void Object::SetColor(RGB newColor)
{
	color = newColor;
}


void Object::UpdatePos(float newX, float newY)
{
	SetPos(newX, newY);
}