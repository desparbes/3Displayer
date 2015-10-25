#ifndef LENS_H
#define LENS_H

#include "frame.h"
#include "coord.h"
#include "point.h"
#include "color.h"

typedef struct Lens Lens;

Lens *initLens(char *fileName);
void resetLens(Lens *l);
void updateLens(Lens *l, Frame *camera);
float *getZBuffer(Lens *l);
int getScreenHeight(Lens *l);
int getScreenWidth(Lens *l);
int getWfov(Lens *l);
int getHfov(Lens *l);
Color *getFilter(Lens *l);
int getNearplan(Lens *l);
int getFarplan(Lens *l);
Frame *getPosition(Lens *l);
Coord *getScreenPosition(Lens *l);
void freeLens(Lens *l);    
    
#endif //LENS_H
