#ifndef SCENE_H
#define SCENE_H

#include "point.h"
#include "frame.h"
#include "solid.h"
#include "SDL/SDL.h"
#include "camera.h"

void initScene();
void updateScene(int *stop);
void addSolidToScene(Solid *solid);
void askSolidForScene();
void askEquationForScene();
void removeSolidFromScene();
void drawScene();
void handleArgumentScene(int argc, char *argv[]);
void freeScene(); 

Camera *getCamera();
Point *getLight();
SDL_Surface *getScreen();

#endif // SCENE_H
