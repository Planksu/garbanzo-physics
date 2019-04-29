#include "Object.h"



Object::Object(SDL_Rect rect, Vector2 p, RGB col, float m)
{
	r = rect;
	pos = p;
	color = col;
	mass = m;

	box.min = Vector2(rect.x, rect.y);
	box.max = Vector2(rect.x + rect.w, rect.y + rect.h);

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