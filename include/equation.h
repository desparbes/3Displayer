#ifndef EQUATION_H
#define EQUATION_H

#include "point.h"

int extractEquation(float *minS, float *maxS, int *precisionS,
		    float *minT, float *maxT, int *precisionT,
		    char *x, char *y, char *z, int stringSize, char *eqName);
void getValueFromEquation(char *x, char *y, char *z, 
			  float s, float t, Point *p);

#endif // EQUATION_H
