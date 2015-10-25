#ifndef PROJECT_H
#define PROJECT_H

#include "point.h"
#include "position.h"
#include "color.h"
#include "coord.h"
#include "texture.h"
#include "lens.h"


void projectVertex(Lens *l, const Point *A, const Color *color);
void projectSegment(Lens *l, const Point *A, const Point *B, const Color *color);
void projectTriangle(Lens *l, const Point *A, const Point *B, const Point *C,
		     Texture *triangle,
		     const Position *U, const Position *V, const Position *W,
		     const Point *normalA, const Point *normalB,
		     const Point *normalC);

#endif //PROJECT_H
