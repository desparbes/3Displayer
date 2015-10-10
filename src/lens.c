#include <stdio.h>
#include <stdlib.h>

#include "frame.h"
#include "coord.h"
#include "point.h"

typedef struct {
    Frame position; // Absolute
    Coord screenPosition;
    int screenWidth;
    int screenHeight;
    float *zBuffer;
    float nearplan;
    float farplan;
    int wfov;
    int hfov;
} Lens;

Lens *initLens(Frame *position, Coord *screenPosition, 
	      int screenWidth, int screenHeight, float nearplan, 
	      float farplan, int wfov, int hfov)
{
    Lens *l = malloc(sizeof(Lens));
    l->position = *position;
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

void translateLens(Lens *l, float x, float y, float z)
{
    translateFrame(&l->position, x, y, z);
}

void rotateLens(Lens *l, Frame *cameraPosition, float theta, float phi, float rho)
{
    rotatePointFromFrame(&l->position.O, cameraPosition, theta, phi, rho);
}

void setDirectionLens(Lens *l, Frame *direction)
{
    l->position.i = direction->i;
    l->position.j = direction->j;
    l->position.k = direction->k;
}    

float *getZBuffer(Lens *l)
{
    return l->zBuffer;
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
    
    
