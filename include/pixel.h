#ifndef PIXEL_H
#define PIXEL_H

#include "coord.h"
#include "color.h"
#include "position.h"

typedef struct Pixel {
    Coord coord;
    Color light;
    float depth;
    Position p;
} Pixel;

void setPixel(Pixel *pixel, Coord const *c, float depth,
              Color const *light, Position const *p);

#endif //PIXEL_H
