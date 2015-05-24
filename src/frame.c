#include <math.h>

#include "point.h"
#include "frame.h"
#include "color.h"
#include "project.h"

void resetFrame(Frame *frame, float x, float y, float z)
{
    setPoint(&frame->O, x, y, z);
    setPoint(&frame->i, 1., 0., 0.);
    setPoint(&frame->j, 0., 1., 0.);
    setPoint(&frame->k, 0., 0., 1.);
}    

void rotateFrame(Frame *frame, float theta, float phi, float rho)
{
    frame->j.x = sin(theta);
    frame->j.y = cos(theta);
    frame->j.z = 0.;

    frame->i.x = frame->j.y;
    frame->i.y = -frame->j.x;
    frame->i.z = 0.;

    Point a;
    rotPoint(&frame->j, &frame->i, phi, &a);
    frame->j = a;
    rotPoint(&frame->i, &frame->j, rho, &a);
    frame->i = a;
    pointProduct(&frame->i, &frame->j, &frame->k);
}

void translateFrame(Frame *frame, float x, float y, float z)
{
    translatePoint(&frame->O, x, y, z);
}
void drawFrame(Frame *frame)
{
    Color color;
    projectSegment(&frame->O, &frame->i, setColor(&color, 255, 0, 0));
    projectSegment(&frame->O, &frame->j, setColor(&color, 0, 255, 0));
    projectSegment(&frame->O, &frame->k, setColor(&color, 0, 0, 255));
}


