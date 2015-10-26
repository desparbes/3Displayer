#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "frame.h"
#include "coord.h"
#include "point.h"
#include "color.h"

#define MAXLENGTH 256
#define NB_KEYWORDS 13

typedef struct {
    Frame position; //Absolute
    Point offset; //Relative to camera
    float theta, phi, rho; //Relative to camera
    Color filter;
    Coord screenPosition;
    int screenWidth;
    int screenHeight;
    float *zBuffer;
    float nearplan;
    float farplan;
    int wfov;
    int hfov;
} Lens;

static void loadDefaultLens(Lens *l)
{
    setPoint(&l->offset, 0.,0.,0.);
    l->theta = 0.;
    l->phi = 0.;
    l->rho = 0.;
    setColor(&l->filter, 255, 255, 255);
    setCoord(&l->screenPosition, 0, 0);
    l->screenWidth = 1200;
    l->screenHeight = 800;
    l->nearplan = 1.;
    l->farplan = 20.;
    l->wfov = 60;
    l->hfov = 80;
}

Lens *initLens(char *fileName)
{
    Lens *l = malloc(sizeof(Lens));
    int checkCount = 0;
    FILE *file = fopen(fileName, "r");

    if (file == NULL) {
        printf("File %s not found", fileName);
    } else {
	char str[MAXLENGTH];
	while (fscanf(file, "%s", str) != EOF) {
	    if (strcmp(str, "offset") == 0 && 
		fscanf(file, "(%f,%f,%f)", 
		       &l->offset.x, &l->offset.y, &l->offset.z) == 3)
		checkCount++;
	    else if (strcmp(str, "theta") == 0 &&
		     fscanf(file, "%f", &l->theta) == 1)
		checkCount++;
	    else if (strcmp(str, "phi") == 0 &&
		     fscanf(file, "%f", &l->phi) == 1)
		checkCount++;
	    else if (strcmp(str, "rho") == 0 &&
		     fscanf(file, "%f", &l->rho) == 1)	    
		checkCount++;
	    else if (strcmp(str, "filter") == 0 && 
		     fscanf(file, "(%hhd,%hhd,%hhd)", 
			    &l->filter.r, 
			    &l->filter.g, 
			    &l->filter.b) == 3) {
		checkCount++;
	    } else if (strcmp(str, "screenPositionWidth") == 0 &&
		       fscanf(file, "%d", &l->screenPosition.w) == 1) {
		checkCount++;
	    } else if (strcmp(str, "screenPositionHeight") == 0 &&
		       fscanf(file, "%d", &l->screenPosition.h) == 1) {
		checkCount++;
	    } else if (strcmp(str, "screenWidth") == 0 &&
		       fscanf(file, "%d", &l->screenWidth) == 1) {
		checkCount++;
	    } else if (strcmp(str, "screenHeight") == 0 &&
		       fscanf(file, "%d", &l->screenHeight) == 1) {
		checkCount++;
	    } else if (strcmp(str, "nearplan") == 0 &&
		       fscanf(file, "%f", &l->nearplan) == 1)	    
		checkCount++;
	    else if (strcmp(str, "farplan") == 0 &&
		     fscanf(file, "%f", &l->farplan) == 1)	    
		checkCount++;
	    else if (strcmp(str, "wfov") == 0 &&
		     fscanf(file, "%d", &l->wfov) == 1)	    
		checkCount++;
	    else if (strcmp(str, "hfov") == 0 &&
		     fscanf(file, "%d", &l->hfov) == 1)	    
		checkCount++;
	}
	fclose(file);
    }
    
    if (checkCount != NB_KEYWORDS) {
	printf("Error parsing lens %s: default lens loaded\n", fileName);
	loadDefaultLens(l);
    } else
	printf("Lens %s successfully loaded\n", fileName);

    initFrame(&l->position);
    l->zBuffer = malloc(sizeof(float) * l->screenHeight * l->screenWidth);
    return l;
}

void resetLens(Lens *l)
{
    int size = l->screenWidth * l->screenHeight;
    for (int i = 0; i < size; i++)
	l->zBuffer[i] = -1.;
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

int getScreenHeight(Lens *l)
{
    return l->screenHeight;
}

int getScreenWidth(Lens *l)
{
    return l->screenWidth;
}

int getNearplan(Lens *l)
{
    return l->nearplan;
}

int getFarplan(Lens *l)
{
    return l->farplan;
}

int getWfov(Lens *l)
{
    return l->wfov;
}

int getHfov(Lens *l)
{
    return l->hfov;
}

Frame *getPosition(Lens *l)
{
    return &l->position;
}

Coord *getScreenPosition(Lens *l)
{
    return &l->screenPosition;
}
    
void freeLens(Lens *l)
{
    free(l->zBuffer);
    free(l);
}
