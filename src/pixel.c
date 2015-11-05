#include "coord.h"
#include "color.h"
#include "position.h"
#include "pixel.h"

void setPixel(Pixel *pixel, const Coord *c, float depth, const Color *light, 
	      const Position *p)
{
    pixel->c = *c;
    pixel->depth = depth;
    pixel->light = *light;
    pixel->p = *p;
}
