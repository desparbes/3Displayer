#include "coord.h"
#include "color.h"
#include "position.h"
#include "pixel.h"

void setPixel(Pixel *pixel, Coord const *c, float depth,
              Color const *light, Position const *p)
{
    pixel->coord = *c;
    pixel->depth = depth;
    pixel->light = *light;
    pixel->p = *p;
}
