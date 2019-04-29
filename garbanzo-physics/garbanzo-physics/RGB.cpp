#include "RGB.h"

RGB::RGB(int red, int green, int blue, int alpha)
{
	r = red;
	g = green;
	b = blue;
	a = alpha;
}


RGB::RGB()
{
	r = 0;
	g = 0;
	b = 0;
	a = 255;
}

RGB::~RGB()
{

}
