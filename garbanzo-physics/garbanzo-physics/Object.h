#ifndef _OBJECT_HEADER_
#define _OBJECT_HEADER_

#include "Vector2.h"
#include "RGB.h"
#include <SDL.h>

struct AABB
{
	Vector2 min;
	Vector2 max;
};

class Object
{
private:
	// Visual representation variables
	Vector2 pos;
	SDL_Rect r;
	RGB color;
	AABB box;

	// Physics variables
	float mass;
	Vector2 velocity;
	float restitution;


	void SetPos(float x, float y)
	{
		// Update virtual coordinates
		pos.SetX(x);
		pos.SetY(y);

		// Update rect coordinates
		r.x = pos.GetX();
		r.y = pos.GetY();
	}
	void SetDimensions(float w, float h)
	{
		r.w = w;
		r.h = h;
	}

public:
	Object(SDL_Rect rect, Vector2 p, RGB col, float m);
	~Object();


	Vector2 GetPos() { return pos; }
	SDL_Rect GetRect() { return r; }
	RGB GetColor() { return color; }
	AABB GetBox() { return box; }
	
	float GetMass() { return mass; }
	Vector2 GetVelocity() { return velocity; }
	float GetRestitution() { return restitution; }

	void SetColor(RGB newColor);
	void UpdatePos(float newX, float newY);

};

#endif
