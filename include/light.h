#ifndef LIGHT_H
#define LIGHT_H

#include "point.h"
#include "color.h"

typedef struct Light Light;

Light *loadLight(char *fileName);
void calculateLight(const Light *l, const Point *A, const Point *nA, Color *c);
void freeLight(Light *l);

#endif // LIGHT_H
