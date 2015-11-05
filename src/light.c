#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "point.h"
#include "color.h"
#include "array.h"

#define MAXLENGTH 256
#define NB_KEYWORDS 5

enum {POSITION, DIRECTION, INTENSITY, ANGLE, COLOR};

typedef struct {
    Point position;
    Point direction; //normalized 
    float intensity;
    float angle; //radian
    Color color;
} Light;

static inline float degreeToRadian(int d)
{
    return d * M_PI / 180.;
}

static float formatScale(float scale)
{
    if (scale >= 0)
	return 0;
    if (scale <= -1)
	return 1;
    return -scale;
}

Light *loadLight(char *fileName)
{
    Light *l = malloc(sizeof(Light));
    int check[NB_KEYWORDS] = {0};
    int template[NB_KEYWORDS];
    int tmp;
    initArray(template, NB_KEYWORDS, 1);
    FILE *file = fopen(fileName, "r");

    if (file == NULL) {
        printf("File %s not found\n", fileName);
    } else {
	char str[MAXLENGTH];
	while (fscanf(file, "%s", str) != EOF) {
	    if (strcmp(str, "position") == 0 && 
		fscanf(file, "%f %f %f", 
		       &l->position.x, &l->position.y, &l->position.z) == 3)
		check[POSITION]++;
	    else if (strcmp(str, "direction") == 0 && 
		     fscanf(file, "%f %f %f", 
			    &l->direction.x, 
			    &l->direction.y, 
			    &l->direction.z) == 3)
		check[DIRECTION]++;
	    else if (strcmp(str, "intensity") == 0 &&
		     fscanf(file, "%f", &l->intensity) == 1)
		check[INTENSITY]++;
	    else if (strcmp(str, "angle") == 0 &&
		     fscanf(file, "%d", &tmp) == 1)
		check[ANGLE]++;
	    else if (strcmp(str, "color") == 0 && 
		     fscanf(file, "%hhd %hhd %hhd", 
			    &l->color.r, 
			    &l->color.g, 
			    &l->color.b) == 3)
		check[COLOR]++;
	    
	}
	fclose(file);
    }
    
    if (!areEqualsArray(check, template, NB_KEYWORDS)) {
	printf("Error parsing light %s\n", fileName);
	free(l);
	return NULL;
    }
    l->angle = degreeToRadian(tmp);
    normalizePoint(&l->direction, &l->direction);
    printf("Light %s successfully loaded\n", fileName);
    return l;
}

void calculateLight(const Light *l, const Point *A, const Point *nA, Color *c)
{
    if (!l) {
	setColor(c, 0, 0, 0);
	return;
    }

    Point OA;
    diffPoint(A, &l->position, &OA);
    float dOA = normPoint(&OA);
    float angle = acos(scalarProduct(&l->direction, &OA) / dOA);
    if (l->angle < 0) { // infinite
	scaleColor(c, &l->color, formatScale(l->intensity * 
					    scalarProduct(&l->direction, nA)));
    } else if (l->angle > angle) { // conic
	scaleColor(c, &l->color, formatScale(l->intensity * 
					    scalarProduct(&OA, nA) / 
					    (dOA * dOA * dOA)));
    } else {
	setColor(c, 0, 0, 0);
    }
}

void freeLight(Light *l)
{
    free(l);
}
