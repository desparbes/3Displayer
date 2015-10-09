#ifndef SOLID_H
#define SOLID_H

#include "color.h"
#include "point.h"
#include "lens.h"
#include "frame.h"

typedef struct Solid Solid;

Solid *loadSolid(const char *fileName, const char *bmpName);
Solid *equationSolid(const char *eqName, const char *bmpName);
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
Point *getOriginSolid(Solid *solid);

void freeSolid(Solid *solid);


#endif //SOLID_H
