#ifndef SOLID_H
#define SOLID_H

#include "color.h"
#include "point.h"

typedef struct Solid Solid;

Solid *loadSolid(const char *fileName, const char *bmpName);
Solid *equationSolid(const char *bmpName,
		     float minS, float maxS, int precisionS,
		     float minT, float maxT, int precisionT);

void wireframeSolid(const Solid *solid, const Color *color);
void vertexSolid(const Solid *solid, const Color *color);
void normalSolid(const Solid *solid, const Color *color);
void drawSolid(const Solid *solid);

void scaleSolid(Solid *solid, const Point *O, float scale);
void rotSolidXAxis(Solid *solid, const Point *O, float phi);
void rotSolidYAxis(Solid *solid, const Point *O, float rho);
void rotSolidZAxis(Solid *solid, const Point *O, float theta);
void translateSolid(Solid *solid, float x, float y, float z);
Point *getOriginSolid(Solid *solid);

void freeSolid(Solid *solid);


#endif //SOLID_H
