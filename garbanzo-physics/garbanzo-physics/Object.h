#ifndef _OBJECT_HEADER_
#define _OBJECT_HEADER_

#include "Vector2.h"
#include "RGB.h"
#include "Rigidbody.h"
#include <SDL.h>

// AABB struct using min as lower left corner and max as top right corner
struct AABB
{
	Vector2 pos;
	Vector2 size;
};

class Object
{
private:
	RGB color;
	AABB box;

	void SetPos(float x, float y)
	{
		// Update rigidbody coordinates
		rb->position.x = x;
		rb->position.y = y;

		// Box position is top left corner
		box.pos.x = rb->position.x - box.size.x / 2;
		box.pos.y = rb->position.y - box.size.y / 2;
	}

public:
	Object(Rigidbody* rb, RGB col, Vector2 size);
	~Object();

	Rigidbody* rb;

	RGB GetColor() { return color; }
	AABB GetBox() { return box; }


	void SetColor(RGB newColor);
	void SetVelocity(Vector2 newVel)
	{
		rb->velocity = newVel;
	}
	void UpdatePos(float newX, float newY);
	void UpdatePos();

};

#endif
