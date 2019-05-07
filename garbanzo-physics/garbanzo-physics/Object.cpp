#include "Object.h"



Object::Object(SDL_Rect rect, Vector2 p, RGB col, float m, float e)
{
	r = rect;
	pos = p;
	color = col;
	mass = m;
	restitution = e;
	velocity = Vector2(0, 0);

	box.pos = p;
	box.size = Vector2(rect.w, rect.h);

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