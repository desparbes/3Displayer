#ifndef SOLID_H
#define SOLID_H

#include "data.h"

Point *getOriginSolid(Solid *solid);
int loadSolid(Solid *solid, const char *fileName);
void equationSolid(Solid *solid,
		   float minS, float maxS, int precisionS,
		   float minT, float maxT, int precisionT);

void wireframeSolid(const Solid *solid, const Color *color);
void vertexSolid(const Solid *solid, const Color *color);
void segmentSolid(const Solid *solid, const Color *color);
void normalSolid(const Solid *solid, const Color *color);
void drawSolid(const Solid *solid);

void freeSolid(Solid *solid);

#endif //SOLID_H
