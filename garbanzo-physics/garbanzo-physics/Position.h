#ifndef _POSITION_HEADER_
#define _POSITION_HEADER_


class Position
{
private:
	float x;
	float y;

public:
	Position(float xPos, float yPos);
	Position();
	~Position();

	float GetX() const { return x; }
	float GetY() const { return y; }

	void SetX(float newX);
	void SetY(float newY);
};

#endif
