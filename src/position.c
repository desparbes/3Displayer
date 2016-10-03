#include "position.h"

static float loopFloat(float x)
{
    if (x < 0.)
        return x - (int) x + 1.;
    else if (x >= 1)
        return x - (int) x;
    return x;
}

void loopPosition(Position *t)
{
    t->x = loopFloat(t->x);
    t->y = loopFloat(t->y);
}

void setPosition(Position *t, float x, float y)
{
    t->x = x;
    t->y = y;
}
