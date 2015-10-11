#ifndef CAMERA_H
#define CAMERA_H

#include "frame.h"
#include "coord.h"
#include "point.h"
#include "lens.h"

typedef struct Camera Camera;

Camera *initCamera(Frame *position);
void addLensToCamera(Camera *c, Point *offset, Coord *screenPosition, 
		     int screenWidth, int screenHeight, float nearplan,
		     float farplan, int wfov, int hfov);
void resetCamera(Camera *c);
void translateCamera(Camera *c, float x, float y, float z);
void rotateCamera(Camera *c, float theta, float phi, float rho);
void removeLensFromCamera(Camera *c);
void initIteratorCamera(Camera *c);
int condIteratorCamera(Camera *c);
void nextIteratorCamera(Camera *c);
Lens *lensIteratorCamera(Camera *c);
Frame *getPositionCamera(Camera *c);
void freeCamera(Camera *c);

#endif //CAMERA_H
