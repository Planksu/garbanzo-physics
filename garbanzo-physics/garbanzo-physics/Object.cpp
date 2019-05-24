#include "Object.h"


Object::Object(Rigidbody* rigidbody, RGB col, Vector2 size)
{
	rb = rigidbody;
	color = col;

	box.size = size;
	UpdateBoxPos();

	// Since the rigidbody values have been given, calculate moment of inertia
	CalculateMomentOfInertia();
}


Object::~Object()
{
}

void Object::SetColor(RGB newColor)
{
	color = newColor;
}


void Object::UpdateRotation()
{
	float s = sin(rb->orientation);
	float c = cos(rb->orientation);

	std::vector<Vector2> corners;


	corners.push_back(box.topLeft);
	corners.push_back(box.topRight);
	corners.push_back(box.bottomLeft);
	corners.push_back(box.bottomRight);
	corners.push_back(box.topNormal);
	corners.push_back(box.leftNormal);

	// Rotate all four corners
	for (size_t i = 0; i < corners.size(); i++)
	{
		// Translate corner to origin
		corners[i].x -= rb->position.x;
		corners[i].y -= rb->position.y;

		float xNew = corners[i].x * c - corners[i].y * s;
		float yNew = corners[i].x * s + corners[i].y * c;

		// Translate back and update coordinates
		corners[i].x = xNew + rb->position.x;
		corners[i].y = yNew + rb->position.y;
	}

	box.topLeft = corners[0];
	box.topRight = corners[1];
	box.bottomLeft = corners[2];
	box.bottomRight = corners[3];
	box.topNormal = corners[4];
	box.leftNormal = corners[5];

}

void Object::UpdateBoxPos()
{
	box.topLeft.x = rb->position.x - box.size.x / 2;
	box.topLeft.y = rb->position.y - box.size.y / 2;
	box.topRight.x = rb->position.x + box.size.x / 2;
	box.topRight.y = rb->position.y - box.size.y / 2;
	box.bottomLeft.x = rb->position.x - box.size.x / 2;
	box.bottomLeft.y = rb->position.y + box.size.y / 2;
	box.bottomRight.x = rb->position.x + box.size.x / 2;
	box.bottomRight.y = rb->position.y + box.size.y / 2;
	box.topNormal = Vector2(rb->position.x, rb->position.y - box.size.y / 2);
	box.leftNormal = Vector2(rb->position.x - box.size.x / 2, rb->position.y);
}