#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "point.h"
#include "color.h"
#include "array.h"

#define MAXLENGTH 256
#define NB_KEYWORDS 6

enum {POSITION, DIRECTION, INTENSITY, INNER, OUTER, COLOR};

typedef struct {
    Point position;
    Point direction; //normalized
    float intensity;
    float inner; //radian
    float outer; //radian
    Color color;
} Light;

static inline float degreeToRadian(int d)
{
    return d * M_PI / 180.;
}

#define FORMAT_SCALE(scale) ((scale) >=0 ? 0.f : ((scale)<=-1 ? 1 : -(scale)))

Light *loadLight(char *fileName)
{
    Light *l = malloc(sizeof(Light));
    int check[NB_KEYWORDS] = {0};
    int template[NB_KEYWORDS];
    int inner, outer;
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
            else if (strcmp(str, "inner") == 0 &&
                     fscanf(file, "%d", &inner) == 1)
                check[INNER]++;
            else if (strcmp(str, "outer") == 0 &&
                     fscanf(file, "%d", &outer) == 1)
                check[OUTER]++;
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
    l->inner = degreeToRadian(inner);
    l->outer = degreeToRadian(outer);
    normalizePoint(&l->direction, &l->direction);
    printf("Light %s successfully loaded\n", fileName);
    return l;
}

void calculateLight(const Light *l, const Point *A, const Point *nA, Color *c)
{
    Point OA;
    diffPoint(A, &l->position, &OA);
    float dOA = normPoint(&OA);
    float angle = acos(scalarProduct(&l->direction, &OA) / dOA);
    float scale;

    if (l->inner < 0) { // infinite
        scale = FORMAT_SCALE(l->intensity * scalarProduct(&l->direction, nA));
        SCALE_COLOR(*c, l->color, scale);
    } else if (angle < l->inner) { // conic
        scale = FORMAT_SCALE(
            l->intensity * scalarProduct(&OA, nA) / (dOA * dOA * dOA));
        SCALE_COLOR(*c, l->color, scale);
    } else if (angle < l->outer) { // conic shade
        scale = FORMAT_SCALE(l->intensity *
                               ((angle - l->outer) / (l->inner - l->outer)) *
                               scalarProduct(&OA, nA) / (dOA * dOA * dOA));
        SCALE_COLOR(*c, l->color, scale);
    } else
        SET_COLOR(*c, 0, 0, 0);
}

void freeLight(Light *l)
{
    free(l);
}
