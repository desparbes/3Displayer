#ifndef CAMERA_H
#define CAMERA_H

typedef struct Camera Camera;
typedef enum direction direction_t;
typedef enum state state_t;

#include "frame.h"
#include "coord.h"
#include "point.h"
#include "lens.h"
#include "color.h"

enum direction {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    DOWN, 
    UP,
    NB_DIRECTION,
};

enum state {
    DRAW,
    WIREFRAME,
    NORMAL,
    VERTEX,
    FRAME,
    NB_STATE,
};

Camera *initCamera(char *fileName);
void resetCamera(Camera *c);
void refreshCamera(Camera *c, int screenWidth, int screenHeight);
void translateCamera(Camera *c, int direction);
void rotateCamera(Camera *c, int direction);
void switchStateCamera(Camera *c, int state);
Lens *getLensOfCamera(Camera *c, int lens);
int getStateCamera(Camera *c, int state);
int getNbLens(Camera *c);
void freeCamera(Camera *c);

#endif //CAMERA_H
