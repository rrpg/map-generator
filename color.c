#include "color.h"

s_color color(int r, int g, int b)
{
	s_color c;
	c.v[0] = r;
	c.v[1] = g;
	c.v[2] = b;

	return c;
}

