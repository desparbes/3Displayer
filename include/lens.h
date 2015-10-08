#ifndef LENS_H
#define LENS_H

#include "frame.h"
#include "coord.h"
#include "point.h"

typedef struct Lens Lens;

void initLens(Lens *l, Frame *position, Frame *parent, Coord *screenPosition, 
	      int screenWidth, int screenHeight, float nearplan, 
	      float farplan, int wfov, int hfov);
void resetLens(Lens *l);
void translateLens(Lens *l, float x, float y, float z);
void rotateLens(Lens *l, float theta, float phi, float rho);
float *getZBuffer(Lens *l);
int getScreenHeight(Lens *l);
int getScreenWidth(Lens *l);
int getWfov(Lens *l);
int getHfov(Lens *l);
int getNearplan(Lens *l);
int getFarplan(Lens *l);
Frame *getPosition(Lens *l);
Coord *getScreenPosition(Lens *l);
void freeLens(Lens *l);    
    
#endif //LENS_H
