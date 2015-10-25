#ifndef FRAME_H
#define FRAME_H

#include "point.h"

typedef struct Frame {
    Point O;
    Point i;
    Point j;
    Point k;
} Frame;


void initFrame(Frame *f);
void rotateFrameAroundPoint(Frame *f, const Point *axis, float angle);
void rotateFrame(Frame *frame, float theta, float phi, float rho);
void translateFrame(Frame *frame, Point *A, float scale);
void getAbsolutePointFromFrame(const Frame *f, const Point *A, Point *B);

#endif // FRAME_H 
