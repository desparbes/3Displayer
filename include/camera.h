#ifndef CAMERA_H
#define CAMERA_H

#include "frame.h"
#include "coord.h"
#include "point.h"
#include "lens.h"

typedef struct Camera Camera;

Camera *initCamera(char *fileName);
void resetCamera(Camera *c);
void translateCamera(Camera *c, int direction);
void rotateCamera(Camera *c, int direction);
void switchStateCamera(Camera *c, int state);
int displayLensCamera(Camera *c, int lens);
Lens *getLensOfCamera(Camera *c, int lens);
int getStateCamera(Camera *c, int state);
int getNbLens(Camera *c);
void freeCamera(Camera *c);

#endif //CAMERA_H
