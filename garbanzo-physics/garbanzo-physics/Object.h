#ifndef _OBJECT_HEADER_
#define _OBJECT_HEADER_

#include "Position.h"
#include "RGB.h"
#include <SDL.h>

class Object
{
private:
	Position pos;
	SDL_Rect r;
	RGB color;

	float mass;

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
	Object(SDL_Rect rect, Position p, RGB col, float m);
	~Object();


	Position GetPos() { return pos; }
	SDL_Rect GetRect() { return r; }
	RGB GetColor() { return color; }
	float GetMass() { return mass; }

	void SetColor(RGB newColor);
	void UpdatePos(float newX, float newY);

};

#endif
