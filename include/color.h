#ifndef COLOR_H
#define COLOR_H

typedef struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

Color *setColor(Color *color, unsigned char r, unsigned char g, unsigned char b);

#endif // COLOR_H
