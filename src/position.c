#include "position.h"










#include <stdio.h>

static float loopFloat(float x)
{
    if (x == -0.)
	return 0;
    else if (x < 0.)
	return x - (int) x + 1.;
    else if (x >= 1)
	return x - (int) x;
    return x;
}

void loopPosition(Position *t)
{
    printf("x: %f, y: %f\n", t->x, t->y);
    t->x = loopFloat(t->x);
    t->y = loopFloat(t->y);
    printf("x: %f, y: %f\n", t->x, t->y);
}

void setPosition(Position *t, float x, float y)
{
    t->x = x;
    t->y = y;
}
