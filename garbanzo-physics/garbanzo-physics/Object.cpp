#include "Object.h"


Object::Object(Rigidbody* rigidbody, RGB col, Vector2 size)
{
	rb = rigidbody;
	color = col;

	box.size = size;
	// Box position is top left corner
	box.pos.x = rb->position.x - size.x / 2;
	box.pos.y = rb->position.y - size.y / 2;

	SetPos(rb->position.x, rb->position.y);
}


Object::~Object()
{
}

void Object::SetColor(RGB newColor)
{
	color = newColor;
}


void Object::UpdatePos()
{
	SetPos(rb->position.x, rb->position.y);
}