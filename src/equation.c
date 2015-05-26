#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "equation.h"
#include "point.h"

int extractEquation(float *minS, float *maxS, int *precisionS,
		    float *minT, float *maxT, int *precisionT,
		    char *x, char *y, char *z, int stringSize, char *eqName)
{
    FILE *file = fopen(eqName, "r");

    if (file == NULL) {
	perror(eqName);
        return 0;
    }
    
    *minS = 0.;
    *maxS = 1.;
    *precisionS = 2;

    *minT = 0.;
    *maxT = 1.;
    *precisionT = 2;
    
    fclose(file);

    return 1;
}

void getValueFromEquation(char *x, char *y, char *z, 
			  float s, float t, Point *p)
{
    //sphere
    //setPoint(p, sin(s) * cos(t), cos(s) * cos(t), sin(t));
    
    //setPoint(p, s, t, 1 / (1 + s*s + t*t));

    //Möbius
    //setPoint(p,
    //         (2 + t * cos(s)) * cos(2*s),
    //	       (2 + t * cos(s)) * sin(2*s),
    //	        t * sin(s));

    // torus
    setPoint(p,
	     sin(s) * (2 + cos(t)),
	     cos(s) * (2 + cos(t)),
	     -sin(t));

    //paraboloid
    //setPoint(p, s, t, t*t - s*s);
}
