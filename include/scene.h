#ifndef SCENE_H
#define SCENE_H

#include "point.h"
#include "solid.h"
#include "SDL/SDL.h"

void initScene();
void addSolidToScene(Solid *solid);
void askSolidForScene();
void askEquationForScene();
void removeSolidFromScene();
void drawScene();
void handleArgumentScene(int argc, char *argv[]);
void rotateCameraScene(int direction);
void translateCameraScene(int direction);
void switchStateCameraScene(int state);
void freeScene(); 

Point *getLight();

#endif // SCENE_H
