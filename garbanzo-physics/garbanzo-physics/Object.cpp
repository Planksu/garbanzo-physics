#include "Object.h"



Object::Object(SDL_Rect rect, Vector2 p, RGB col, float m, Vector2 f, Vector2 vel, float a, float o, float angVel, float torq, float e)
{
	r = rect;
	pos = p;
	color = col;
	mass = m;
	force = f;
	velocity = vel;
	acceleration = a;
	orientation = o;
	angularVelocity = angVel;
	torque = torq;
	restitution = e;

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