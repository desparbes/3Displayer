#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "frame.h"
#include "coord.h"
#include "point.h"
#include "color.h"
#include "array.h"
#include "display.h"

#define MAXLENGTH 256
#define NB_KEYWORDS 13
#define MAXWINDOWS 8
#define SQRMAXWINDOWS (MAXWINDOWS * MAXWINDOWS)

enum {OFFSET, THETA, PHI, RHO, FILTER, WIDTHPOSITION, HEIGHTPOSITION,
      SCREENWIDTH, SCREENHEIGHT, OVERLAPPING, NEARPLAN, FARPLAN, WFOV};

typedef struct {
    Frame position; //Absolute
    Point offset; //Relative to camera
    float theta, phi, rho; //Relative to camera
    Color filter;
    int widthPositionA;
    int widthPosition; //Relative
    int heightPositionA;
    int heightPosition; //Relative
    int screenWidthA;
    int screenWidth; //Relative
    int screenHeightA;
    int screenHeight; //Relative
    int overlapping;
    float *zBuffer;
    float nearplan;
    float farplan;
    float wfov; //Absolute
    float hfov; //Relative
} Lens;

static inline int isInRange(int n)
{
    return (n <= MAXWINDOWS && n >= 0);
}

static inline float degreeToRadian(int d)
{
    return d * M_PI / 180.;
}

static void loadDefaultLens(Lens *l)
{
    setPoint(&l->offset, 0.,0.,0.);
    l->overlapping = 0;
    l->theta = 0.;
    l->phi = 0.;
    l->rho = 0.;
    setColor(&l->filter, 255, 255, 255);
    l->widthPosition = 0;
    l->heightPosition = 0;
    l->screenWidth = 8;
    l->screenHeight = 8;
    l->nearplan = 1.;
    l->farplan = 20.;
    l->wfov = degreeToRadian(90);
}

Lens *initLens(char *fileName)
{
    Lens *l = malloc(sizeof(Lens));
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
	    if (strcmp(str, "offset") == 0 && 
		fscanf(file, "%f %f %f", 
		       &l->offset.x, &l->offset.y, &l->offset.z) == 3)
		check[OFFSET]++;
	    else if (strcmp(str, "theta") == 0 &&
		     fscanf(file, "%f", &l->theta) == 1)
		check[THETA]++;
	    else if (strcmp(str, "phi") == 0 &&
		     fscanf(file, "%f", &l->phi) == 1)
		check[PHI]++;
	    else if (strcmp(str, "rho") == 0 &&
		     fscanf(file, "%f", &l->rho) == 1)	    
		check[RHO]++;
	    else if (strcmp(str, "filter") == 0 && 
		     fscanf(file, "%hhd %hhd %hhd", 
			    &l->filter.r, 
			    &l->filter.g, 
			    &l->filter.b) == 3) {
		check[FILTER]++;
	    } else if (strcmp(str, "screenPositionWidth") == 0 &&
		       fscanf(file, "%d", &l->widthPosition) == 1 &&
		       isInRange(l->widthPosition)) {
		check[WIDTHPOSITION]++;
	    } else if (strcmp(str, "screenPositionHeight") == 0 &&
		       fscanf(file, "%d", &l->heightPosition) == 1 &&
		       isInRange(l->heightPosition)) {
		check[HEIGHTPOSITION]++;
	    } else if (strcmp(str, "screenWidth") == 0 &&
		       fscanf(file, "%d", &l->screenWidth) == 1 &&
		       isInRange(l->screenWidth)) {
		check[SCREENWIDTH]++;
	    } else if (strcmp(str, "screenHeight") == 0 &&
		       fscanf(file, "%d", &l->screenHeight) == 1 &&
		       isInRange(l->screenHeight)) {
		check[SCREENHEIGHT]++;
	    } else if (strcmp(str, "overlapping") == 0 &&
		       fscanf(file, "%d", &l->overlapping) == 1) {
		check[OVERLAPPING]++;
	    } else if (strcmp(str, "nearplan") == 0 &&
		       fscanf(file, "%f", &l->nearplan) == 1)	    
		check[NEARPLAN]++;
	    else if (strcmp(str, "farplan") == 0 &&
		     fscanf(file, "%f", &l->farplan) == 1)	    
		check[FARPLAN]++;
	    else if (strcmp(str, "wfov") == 0 &&
		     fscanf(file, "%d", &tmp) == 1)	    
		check[WFOV]++;
	}
	fclose(file);
    }
    
    if (!areEqualsArray(check, template, NB_KEYWORDS)) {
	printf("Error parsing lens %s: default lens loaded\n", fileName);
	loadDefaultLens(l);
    } else {
	l->wfov = degreeToRadian(tmp);
	printf("Lens %s successfully loaded\n", fileName);
    }
    initFrame(&l->position);
    l->zBuffer = NULL;
    return l;
}

void resetLens(Lens *l)
{
    memset(l->zBuffer, 0, sizeof(float) * l->screenWidthA * l->screenHeightA);
}

void refreshLens(Lens *l, int wD, int hD)
{
    l->widthPositionA = l->widthPosition * wD / MAXWINDOWS;
    l->heightPositionA = l->heightPosition * hD / MAXWINDOWS;
    l->screenWidthA = l->screenWidth * wD / MAXWINDOWS;
    l->screenHeightA = l->screenHeight * hD / MAXWINDOWS;
    l->hfov = 2 * atan(tan(l->wfov / 2.) * 
		       ((float)l->screenWidthA / l->screenHeightA));
    l->zBuffer = realloc(l->zBuffer, sizeof(float) * 
			 l->screenWidthA * l->screenHeightA);
}

void updateLens(Lens *l, Frame *camera)
{
    l->position.i = camera->i;
    l->position.j = camera->j;
    l->position.k = camera->k;

    rotateFrameAroundPoint(&l->position, &camera->k, l->theta);
    rotateFrameAroundPoint(&l->position, &camera->i, l->phi);
    rotateFrameAroundPoint(&l->position, &camera->j, l->rho);

    getAbsolutePointFromFrame(camera, &l->offset, &l->position.O);
}    

float *getZBuffer(Lens *l)
{
    return l->zBuffer;
}

Color *getFilter(Lens *l)
{
    return &l->filter;
}

int getScreenWidth(Lens *l)
{
    return l->screenWidthA;
}

int getScreenHeight(Lens *l)
{
    return l->screenHeightA;
}

int getNearplan(Lens *l)
{
    return l->nearplan;
}

int getFarplan(Lens *l)
{
    return l->farplan;
}

float getWfov(Lens *l)
{
    return l->wfov;
}

float getHfov(Lens *l)
{
    return l->hfov;
}

Frame *getPosition(Lens *l)
{
    return &l->position;
}

int getWidthPosition(Lens *l)
{
    return l->widthPositionA;
}

int getHeightPosition(Lens *l)
{
    return l->heightPositionA;
}

int getOverlapping(Lens *l)
{
    return l->overlapping;
}
    
void freeLens(Lens *l)
{
    free(l->zBuffer);
    free(l);
}
