#ifndef PROJECT_H
#define PROJECT_H

#include "point.h"
#include "position.h"
#include "color.h"
#include "coord.h"
#include "texture.h"
#include "lens.h"


void projectVertex(Lens *l, const Point *A, Color color);
void projectSegment(Lens *l, const Point *A, const Point *B, Color color);

void projectTriangle(Lens *l,
                     const Point *A, // vertex position
                     const Point *B,
                     const Point *C,

                     Texture *triangle,

                     const Position *U, // vertex texture coords
                     const Position *V,
                     const Position *W,

                     const Point *normalA, // vertex normals
                     const Point *normalB,
                     const Point *normalC);

#endif //PROJECT_H
