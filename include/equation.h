#ifndef EQUATION_H
#define EQUATION_H

#include "point.h"

int initEquation(float *minS, float *maxS, int *precisionS,
		 float *minT, float *maxT, int *precisionT,
		 char *x, char *y, char *z, int stringSize, 
		 const char *eqName);
int getValueFromEquation(char *x, char *y, char *z, 
			  float s, float t, Point *p);
void freeEquation();


#endif // EQUATION_H
