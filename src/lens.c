#include <stdio.h>
#include <stdlib.h>

#include "lens.h"
#include "frame.h"
#include "coord.h"
#include "point.h"

typedef struct {
    Frame position; // Absolute
    Frame *parent;
    Coord screenPosition;
    int screenWidth;
    int screenHeight;
    float *zBuffer;
    float nearplan;
    float farplan;
    int wfov;
    int hfov;
} Lens;

void initLens(Lens *l, Frame *position, Frame *parent, Coord *screenPosition, 
	      int screenWidth, int screenHeight, float nearplan, 
	      float farplan, int wfov, int hfov)
{
    l->position = *position;
    l->parent = parent;
    l->screenPosition = *screenPosition;
    l->screenWidth = screenWidth;
    l->screenHeight = screenHeight;
    l->zBuffer = malloc(screenWidth * screenHeight * sizeof(float));
    l->nearplan = nearplan;
    l->farplan = farplan;
    l->wfov = wfov;
    l->hfov = hfov;
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

void rotateLens(Lens *l, float theta, float phi, float rho)
{
    rotPoint(&l->position.O, &l->parent->k, theta, &l->position.O);
    rotPoint(&l->position.O, &l->parent->i, phi, &l->position.O);
    rotPoint(&l->position.O, &l->parent->j, rho, &l->position.O);
    
    rotateFrame(&l->position, theta, phi, rho);
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
}
    
    
