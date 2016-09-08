#ifndef COORD_H
#define COORD_H

#include <stdint.h>

typedef struct Coord {
    uint16_t w;
    uint16_t h;
} Coord;

#define SET_COORD(C, w_, h_)                    \
    do{                                         \
        (C).w = (w_);                           \
        (C).h = (h_);                           \
    }while(0)

#define ADD_COORD(C, A, B)                      \
    do{                                         \
        (C).w = (A).w + (B).w;                  \
        (C).h = (A).h + (B).h;                  \
    }while(0)

#define DIFF_COORD(C, A, B)                     \
    do{                                         \
        (C).w = (A).w - (B).w;                  \
        (C).h = (A).h - (B).h;                  \
    }while(0)

#define TRANSLATE_COORD(C, w_, h_)              \
    do{                                         \
        (C).w += (w_);                          \
        (C).h += (h_);                          \
    }while(0)

#define PRODUCT_COORD(U, V)  ((U).h * (V).w - (U).w * (V).h)
#define DISTANCE_COORD(U, V)  (sqrt(SQUARE((U).w-(V).w) + SQUARE((U).h-(V).h)))


#ifdef MAX
#undef MAX
#endif
#define MAX(x,y) ((x) < (y) ? (y) : (x))


#ifdef MIN
#undef MIN
#endif
#define MIN(x,y) ((x) < (y) ? (x) : (y))

#endif // COORD_H
