#ifndef PIXEL_H
#define PIXEL_H

#include "coord.h"
#include "color.h"
#include "position.h"

typedef struct Pixel {
    Coord c;
    float depth;
    Color light;
    Position p;
} Pixel;

void setPixel(Pixel *pixel, const Coord *c, float depth, const Color *light, 
	      const Position *p);
#endif //PIXEL_H
