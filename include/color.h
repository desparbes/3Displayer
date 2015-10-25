#ifndef COLOR_H
#define COLOR_H

typedef struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

Color *setColor(Color *color, unsigned char r, unsigned char g, unsigned char b);
Color *filterColor(Color *color, const Color *filter);
Color *scaleColor(Color *color, float scale);

#endif // COLOR_H
