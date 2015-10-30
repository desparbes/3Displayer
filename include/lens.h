#ifndef LENS_H
#define LENS_H

#include "frame.h"
#include "coord.h"
#include "point.h"
#include "color.h"

typedef struct Lens Lens;

Lens *initLens(char *fileNames);
void resetLens(Lens *l);
void updateLens(Lens *l, Frame *camera);
void refreshLens(Lens *l, int wD, int hD);
float *getZBuffer(Lens *l);
int getScreenHeight(Lens *l);
int getScreenWidth(Lens *l);
float getWfov(Lens *l);
float getHfov(Lens *l);
Color *getFilter(Lens *l);
int getNearplan(Lens *l);
int getFarplan(Lens *l);
Frame *getPosition(Lens *l);
int getWidthPosition(Lens *l);
int getHeightPosition(Lens *l);
void freeLens(Lens *l);    
    
#endif //LENS_H
