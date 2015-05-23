#ifndef DRAW_H
#define DRAW_H

#include "data.h"

void drawPixel(const Coord *A, const Color *color);

void drawSegment(const Coord *A, const Coord *B,
		 float depthA, float depthB,
		 const Color *color);

void drawTriangle(const Coord *A, const Coord *B, const Coord *C,
		  float depthA, float depthB, float depthC,
		  SDL_Surface *triangle,
		  const Texture *U, const Texture *V, const Texture *W,
		  const Point *normalA, const Point *normalB,
		  const Point *normalC);

void drawOrigin(Frame *origin);

 
#endif //DRAW_H
