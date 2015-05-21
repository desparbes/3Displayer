#ifndef PROJECT_H
#define PROJECT_H

#include "data.h"
#include "project.h"
#include "basic.h"
#include "draw.h"

// return the vector between user.position and the intersection of
// (AB) and the NEARPLAN
void projectPoint(const Point *A, const Point *B, Point *S);
// return the Coord projection of a vector between user.position
// and A, knowing the depth of A
void projectCoord(const Point *OA, float depth, Coord *S);
void projectVertex(const Point *A, const Color *color);
void projectSegment(const Point *A, const Point *B, const Color *color);

void projectTriangle(const Point *A, const Point *B, const Point *C,
		     SDL_Surface *triangle,
		     const Texture *U, const Texture *V, const Texture *W,
		     const Point *normalA, const Point *normalB,
		     const Point *normalC);

#endif //PROJECT_H
