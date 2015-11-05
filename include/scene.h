#ifndef SCENE_H
#define SCENE_H

#include "point.h"
#include "solid.h"
#include "color.h"

void initScene();
void askSolidForScene();
void askEquationForScene();
void removeSolidFromScene();
void drawScene();
void handleArgumentScene(int argc, char *argv[]);
void resizeCameraScene(int screenWidth, int screenHeight);
void rotateCameraScene(int direction);
void translateCameraScene(int direction);
void switchStateCameraScene(int state);
void calculateLightScene(const Point *A, const Point *nA, Color *c);
void freeScene(); 

#endif // SCENE_H
