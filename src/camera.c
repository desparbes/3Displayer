#include <stdio.h>
#include <stdlib.h>

#include "lens.h"
#include "frame.h"
#include "coord.h"
#include "point.h"

typedef struct {
    int iterator;
    Frame position;
    Lens **lensBuffer;
    int nbLens;
    int bufferSize;
} Camera;

Camera *initCamera(void)
{
    Camera *c = malloc(sizeof(Camera));
    resetFrame(&c->position, 0., -5., 0.);
    c->nbLens = 0;
    c->bufferSize = 4;
    c->iterator = 0;
    c->lensBuffer = malloc(c->bufferSize * sizeof(Lens *));
    return c;
}

void addLensToCamera(Camera *c, Frame *position, Coord *screenPosition, 
		     int screenWidth, int screenHeight, float nearplan, 
		     float farplan, int wfov, int hfov)
{
    if (c->nbLens >= c->bufferSize){
	c->bufferSize *= 2;
	c->lensBuffer = realloc(c->lensBuffer, c->bufferSize * sizeof(Lens *));
    }
    c->lensBuffer[c->nbLens++] = initLens(position, &c->position, screenPosition,
					  screenWidth, screenHeight, nearplan, 
					  farplan, wfov, hfov);
}

void resetCamera(Camera *c)
{
    for (int i = 0; i < c->nbLens; i++)
	resetLens(c->lensBuffer[i]);
}

void translateCamera(Camera *c, float x, float y, float z)
{
    for (int i = 0; i < c->nbLens; i++)
	translateLens(c->lensBuffer[i], x, y, z);
    translateFrame(&c->position, x, y, z);
}

void rotateCamera(Camera *c, float theta, float phi, float rho)
{
    for (int i = 0; i < c->nbLens; i++)
	rotateLens(c->lensBuffer[i], theta, phi, rho);
    rotateFrame(&c->position, theta, phi, rho);
}

void removeLensFromCamera(Camera *c)
{
    if (c->nbLens > 0)
	freeLens(c->lensBuffer[--c->nbLens]);
}

void initIteratorCamera(Camera *c)
{
    c->iterator = 0;
}

int condIteratorCamera(Camera *c)
{
    return c->iterator < c->nbLens;
}

void nextIteratorCamera(Camera *c)
{
    c->iterator++;
}

Lens *lensIteratorCamera(Camera *c)
{
    return c->lensBuffer[c->iterator];
}

Frame *getPositionCamera(Camera *c)
{
    return &c->position;
}

void freeCamera(Camera *c)
{
    for (int i = 0; i < c->nbLens; i++)	
	freeLens(c->lensBuffer[i]);
    free(c->lensBuffer);
    free(c);
}
