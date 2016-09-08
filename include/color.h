#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>
typedef struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

#define COLOR(r, g, b)  { (r), (g), (b) }

#define SET_COLOR(c, r_, g_, b_)                \
    do{                                         \
        (c).r = (r_);                           \
        (c).g = (g_);                           \
        (c).b = (b_);                           \
    }while(0)

#define SCALE_COLOR(c, A, s)                    \
    do{                                         \
        (c).r = (A).r * (s);                    \
        (c).g = (A).g * (s);                    \
        (c).b = (A).b * (s);                    \
    }while(0)

#define FILTER_COLOR(c, A, F)                   \
    do {                                        \
        ((c).r = (A).r * (F).r / 255.);         \
        ((c).g = (A).g * (F).g / 255.);         \
        ((c).b = (A).b * (F).b / 255.);         \
    }while(0)

#define PRODUCT_COLOR(c, A, B)                                  \
    do {                                                        \
        ((c).r = (unsigned)(A).r * (unsigned)(B).r / 255);      \
        ((c).g = (unsigned)(A).g * (unsigned)(B).g / 255);      \
        ((c).b = (unsigned)(A).b * (unsigned)(B).b / 255);      \
    }while(0)

#define ADD_COLOR(c, A, B)                      \
    do {                                        \
        (c).r = MAX((A).r + (B).r, 255);        \
        (c).g = MAX((A).g + (B).g, 255);        \
        (c).b = MAX((A).b + (B).b, 255);        \
    }while(0)

#define INTERPOLATE_COLOR(c, A, B, C, alpha, beta, gamma)       \
    do {                                                        \
        (c).r = (alpha)*(A).r + (beta)*(B).r + (gamma)*C.r;     \
        (c).g = (alpha)*(A).g + (beta)*(B).g + (gamma)*C.g;     \
        (c).b = (alpha)*(A).b + (beta)*(B).b + (gamma)*C.b;     \
    }while(0)

#define AVERAGE_COLOR(c, F, B, s)                       \
    do {                                                \
        (c).r = (F).r + ((B).r-(F).r)*(s) / 255;        \
        (c).g = (F).g + ((B).g-(F).g)*(s) / 255;        \
        (c).b = (F).b + ((B).b-(F).b)*(s) / 255;        \
    }while(0)

#endif // COLOR_H
