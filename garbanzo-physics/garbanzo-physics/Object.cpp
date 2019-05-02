#include "Object.h"



Object::Object(SDL_Rect rect, Vector2 p, RGB col, float m)
{
	r = rect;
	pos = p;
	color = col;
	mass = m;
	velocity = Vector2(0, 0);

	box.min = Vector2(pos.x - rect.w / 2, pos.y - rect.h / 2);
	box.max = Vector2(pos.x + rect.w/2, pos.y + rect.h/2);

	SetPos(p.x, p.y);
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