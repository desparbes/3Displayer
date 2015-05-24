#ifndef SCENE_H
#define SCENE_H

#include "point.h"
#include "frame.h"
#include "solid.h"
#include "SDL/SDL.h"

void initScene();
void updateScene(int *stop);
void addSolidToScene(Solid *solid);
void removeSolidFromScene(Solid *solid);
void drawScene();
void freeScene(); 

Frame *getCamera();
Point *getLight();
SDL_Surface *getScreen();
int getHfov();
int getWfov();
int getScreenWidth();
int getScreenHeight();
float getNearplan();
float getFarplan();
float *getZBuffer();

#endif // SCENE_H
