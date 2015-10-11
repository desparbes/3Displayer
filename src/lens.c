#include <stdio.h>
#include <stdlib.h>

#include "frame.h"
#include "coord.h"
#include "point.h"
#include "color.h"

typedef struct {
    Frame position; //Absolute
    Point offset; //Relative to camera
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

Lens *initLens(Point *offset, Coord *screenPosition, Color *filter,
	      int screenWidth, int screenHeight, float nearplan, 
	      float farplan, int wfov, int hfov)
{
    Lens *l = malloc(sizeof(Lens));
    l->offset = *offset;
    l->filter = *filter;
    l->screenPosition = *screenPosition;
    l->screenWidth = screenWidth;
    l->screenHeight = screenHeight;
    l->zBuffer = malloc(screenWidth * screenHeight * sizeof(float));
    l->nearplan = nearplan;
    l->farplan = farplan;
    l->wfov = wfov;
    l->hfov = hfov;
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
    absolutePointInFrame(camera, &l->offset, &l->position.O);
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
    
    
