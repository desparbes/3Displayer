#include <math.h>

#include "coord.h"

void setCoord(Coord *c, int w, int h) 
{
    c->w = w;
    c->h = h;
}

void diffRect(const Coord *A, const Coord *B, Coord *S)
{
    setCoord(S,
	     A->w - B->w,
	     A->h - B->h);
}

int relativeProduct(const Coord *u, const Coord *v)
{
    return u->h * v->w - u->w * v->h;
}

float distanceCoord(const Coord * A, const Coord * B)
{
    return sqrt((A->w - B->w) * (A->w - B->w) +
		(A->h - B->h) * (A->h - B->h));
}

