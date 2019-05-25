#ifndef _OBJECT_HEADER_
#define _OBJECT_HEADER_

#include "Vector2.h"
#include "RGB.h"
#include "Rigidbody.h"
#include <vector>
#include <SDL.h>

// AABB struct using min as lower left corner and max as top right corner
struct AABB
{
	// Corners
	Vector2 topLeft;
	Vector2 topRight;
	Vector2 bottomLeft;
	Vector2 bottomRight;

	// Normal points
	// Basically, just the middle point of the top edge and the left edge
	Vector2 topNormal;
	Vector2 leftNormal;

	// Size
	Vector2 size;
};

class Object
{
private:
	RGB color;
	AABB box;

public:
	Object(Rigidbody* rb, RGB col, Vector2 size);
	~Object();

	Rigidbody* rb;
	Vector2 mtv;

	RGB GetColor() { return color; }
	AABB GetBox() { return box; }


	void SetColor(RGB newColor);

	void CalculateMomentOfInertia()
	{
		rb->momentOfInertia = rb->mass * (box.size.x * box.size.x + box.size.y * box.size.y) / 12;
	}

	void UpdateRotation();
	void UpdateBoxPos();
};

#endif
