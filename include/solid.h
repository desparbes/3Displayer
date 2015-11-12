#ifndef SOLID_H
#define SOLID_H

#include "color.h"
#include "point.h"
#include "lens.h"
#include "frame.h"
#include "position.h"
#include "texture.h"
#include "build.h"

Solid *loadSolid(const char *fileName, const char *bmpName);

void calculateOriginSolid(Solid *solid);
Point *getOriginSolid(Solid *solid);

void wireframeSolid(Lens *l, const Solid *solid, const Color *color);
void vertexSolid(Lens *l, const Solid *solid, const Color *color);
void normalSolid(Lens *l, const Solid *solid, const Color *color);
void drawSolid(Lens *l, const Solid *solid);
void drawFrame(Lens *l, Frame *frame);

void scaleSolid(Solid *solid, const Point *O, float scale);
void rotSolidXAxis(Solid *solid, const Point *O, float phi);
void rotSolidYAxis(Solid *solid, const Point *O, float rho);
void rotSolidZAxis(Solid *solid, const Point *O, float theta);
void translateSolid(Solid *solid, float x, float y, float z);

void freeSolid(Solid *solid);


#endif //SOLID_H
