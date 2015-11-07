#include "color.h"

static unsigned char sum(unsigned int a, unsigned int b)
{
    int c = a + b;
    return c < 255 ? c : 255;
}

static unsigned char product(unsigned int a, unsigned int b)
{
    return a * b / 255;
}

Color *setColor(Color *color, unsigned char r, unsigned char g, unsigned char b)
{
    color->r = r;
    color->g = g;
    color->b = b;
    return color;
}

Color *interpolateColor(Color *M, const Color *A, const Color *B, const Color *C,
			float alpha, float beta, float gamma)
{
    M->r = alpha * A->r + beta * B->r + gamma * C->r;
    M->g = alpha * A->g + beta * B->g + gamma * C->g;
    M->b = alpha * A->b + beta * B->b + gamma * C->b;
    return M;
}

Color *sumColor(const Color *A, const Color *B, Color *C)
{
    C->r = sum(A->r, B->r);
    C->g = sum(A->g, B->g);
    C->b = sum(A->b, B->b);
    return C;
}

Color *productColor(const Color *A, const Color *B, Color *C)
{
    C->r = product(A->r, B->r);
    C->g = product(A->g, B->g);
    C->b = product(A->b, B->b);
    return C;
}

Color *filterColor(Color *color, const Color *filter)
{
    color->r = (color->r * filter->r) / 255.;
    color->g = (color->g * filter->g) / 255.;
    color->b = (color->b * filter->b) / 255.;
    return color;
}

Color *scaleColor(Color *output, const Color *input, float scale)
{
    output->r = scale * input->r;
    output->g = scale * input->g;
    output->b = scale * input->b;
    return output;
}
