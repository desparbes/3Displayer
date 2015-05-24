#include "texture.h"

float loopTexture(float a)
{
    if (a < 0)
	return a - (int) a + 1;
    else if (a >= 1)
	return a - (int) a;
    else
	return a;
}

void setTexture(Texture *t, float x, float y)
{
    t->x = x;
    t->y = y;
}
