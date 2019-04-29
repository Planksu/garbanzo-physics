#ifndef _RGB_HEADER_
#define _RGB_HEADER_

class RGB
{
public:
	RGB(int red, int green, int blue, int alpha);
	RGB();
	~RGB();


	int r;
	int g;
	int b;
	int a;
};

#endif

