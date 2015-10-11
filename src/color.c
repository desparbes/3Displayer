#include "color.h"

Color *setColor(Color *color, unsigned char r, unsigned char g, unsigned char b)
{
    color->r = r;
    color->g = g;
    color->b = b;
    return color;
}

void filterColor(Color *color, const Color *filter)
{
    filter->r == 0 ? color->r = 0 : 1;
    filter->g == 0 ? color->g = 0 : 1;
    filter->b == 0 ? color->b = 0 : 1;
}
