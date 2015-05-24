#ifndef FRAME_H
#define FRAME_H

#include "point.h"

typedef struct Frame {
    Point O;
    Point i;
    Point j;
    Point k;
} Frame;

void resetFrame(Frame *frame);
void setDirectionFrame(Frame *frame, float theta, float phi, float rho);
void translateFrame(Frame *frame, float x, float y, float z);
void drawFrame(Frame *frame);

#endif // FRAME_H 
