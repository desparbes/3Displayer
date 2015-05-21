#include <SDL/SDL.h>
#include "math.h"
#include "data.h"

float wCoef;
float hCoef;

Color red = {
    .r = 255,
    .g = 0,
    .b = 0
};

Color green = {
    .r = 0,
    .g = 255,
    .b = 0
};

Color blue = {
    .r = 0,
    .g = 0,
    .b = 255
};

Point light = {
    .x = 1,
    .y = -0.5,
    .z = -2
};

SDL_Surface *screen;
float *zBuffer;
Camera user;

Point PointO = {
    0,
    0,
    0
};

Point I = {
    1,
    0,
    0
};

Point J = {
    0,
    1,
    0
};

Point K = {
    0,
    0,
    1
};


void setPoint(Point *p, float x, float y, float z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}

void setCoord(Coord *c, int w, int h) 
{
    c->w = w;
    c->h = h;
}

void setTexture(Texture *t, float x, float y)
{
    t->x = x;
    t->y = y;
}
