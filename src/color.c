#include "color.h"

Color *setColor(Color *color, unsigned char r, unsigned char g, unsigned char b)
{
    color->r = r;
    color->g = g;
    color->b = b;
    return color;
}
