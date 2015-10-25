#ifndef DRAW_H
#define DRAW_H

#include "point.h"
#include "coord.h"
#include "color.h"
#include "position.h"
#include "texture.h"
#include "lens.h"

void drawPixel(Lens *l, const Coord *A, float depthA, const Color *color);

void drawSegment(Lens *l, const Coord *A, const Coord *B,
		 float depthA, float depthB,
		 const Color *color);

void drawTriangle(Lens *l, const Coord *A, const Coord *B, const Coord *C,
		  float depthA, float depthB, float depthC,
		  Texture *triangle,
		  const Position *U, const Position *V, const Position *W,
		  const Point *normalA, const Point *normalB,
		  const Point *normalC);
 
#endif //DRAW_H
