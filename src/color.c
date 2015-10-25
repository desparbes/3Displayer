#include "color.h"

Color *setColor(Color *color, unsigned char r, unsigned char g, unsigned char b)
{
    color->r = r;
    color->g = g;
    color->b = b;
    return color;
}

Color *filterColor(Color *color, const Color *filter)
{
    color->r *= (float)(filter->r) / 255.;
    color->r *= (float)(filter->r) / 255.;
    color->r *= (float)(filter->r) / 255.;
    return color;
}

Color *scaleColor(Color *color, float scale)
{
    color->r *= scale;
    color->g *= scale;
    color->b *= scale;
    return color;
}
