#ifndef _OBJECT_HEADER_
#define _OBJECT_HEADER_

#include "Vector2.h"
#include "RGB.h"
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
	// Visual representation variables
	SDL_Rect r;
	Vector2 pos;
	RGB color;
	AABB box;

	// Physics variables
	// Linear
	float mass;
	Vector2 force;
	Vector2 velocity;
	float acceleration;

	// Angular
	float orientation;
	float angularVelocity;
	float torque;

	float restitution;


	void SetPos(float x, float y)
	{
		// Update virtual coordinates
		pos.x = x;
		pos.y = y;

		// Update rect coordinates
		r.x = pos.x;
		r.y = pos.y;

		box.pos = Vector2(x, y);
	}
	void SetDimensions(float w, float h)
	{
		r.w = w;
		r.h = h;
	}

public:
	Object(SDL_Rect rect, Vector2 p, RGB col, float m, Vector2 f, Vector2 vel, float a, float o, float angVel, float torq, float e);
	~Object();


	Vector2 GetPos() { return pos; }
	SDL_Rect GetRect() { return { r.x - r.w / 2, r.y - r.h / 2, r.w, r.h }; }
	RGB GetColor() { return color; }
	AABB GetBox() { return box; }
	
	float GetMass() { return mass; }
	Vector2 GetVelocity() { return velocity; }
	float GetRestitution() { return restitution; }

	void SetColor(RGB newColor);
	void SetVelocity(Vector2 newVel)
	{
		velocity = newVel;
	}
	void UpdatePos(float newX, float newY);

};

#endif
