#ifndef COLOR_H
#define COLOR_H

typedef struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

Color *setColor(Color *color, unsigned char r, unsigned char g, unsigned char b);
Color *filterColor(Color *color, const Color *filter);
Color *scaleColor(Color *output, const Color *input, float scale);
Color *sumColor(const Color *A, const Color *B, Color *C);
Color *productColor(const Color *A, const Color *B, Color *C);
Color *interpolateColor(Color *M, const Color *A, const Color *B, const Color *C,
			float alpha, float beta, float gamma);
#endif // COLOR_H
