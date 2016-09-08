#ifndef SCENE_H
#define SCENE_H

#include "point.h"
#include "solid.h"
#include "color.h"
#include "config.h"

void initScene(Config *config);
void askSolidForScene(void);
void removeSolidFromScene(void);
void drawScene(void);
void handleArgumentScene(int argc, char *argv[]);
void resizeCameraScene(int screenWidth, int screenHeight);
void rotateCameraScene(int direction);
void translateCameraScene(int direction);
void switchStateCameraScene(int state);
void calculateLightScene(const Point *A, const Point *nA, Color *c);
void freeScene(void);

#endif // SCENE_H
