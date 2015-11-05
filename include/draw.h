#ifndef DRAW_H
#define DRAW_H

#include "point.h"
#include "coord.h"
#include "color.h"
#include "position.h"
#include "texture.h"
#include "lens.h"
#include "pixel.h"

void drawPixel(Lens *l, const Coord *A, float depthA, const Color *color);

void drawSegment(Lens *l, const Coord *A, const Coord *B,
		 float depthA, float depthB,
		 const Color *color);

void drawTriangle(Lens *l, Texture *triangle, Pixel *A, Pixel *B, Pixel *C);
 
#endif //DRAW_H
