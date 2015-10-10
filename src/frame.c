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

    rotPoint(&frame->j, &frame->i, phi, &frame->j);
    rotPoint(&frame->i, &frame->j, rho, &frame->i);
    pointProduct(&frame->i, &frame->j, &frame->k);
}

void rotatePointFromFrame(Point *A, Frame *frame, 
			  float theta, float phi, float rho)
{
    Point OA;
    diffPoint(A, &frame->O, &OA);
    rotPoint(&OA, &frame->k, theta, &OA);
    rotPoint(&OA, &frame->i, phi, &OA);
    rotPoint(&OA, &frame->j, rho, &OA);
    sumPoint(A, &OA, A);
}

void translateFrame(Frame *frame, float x, float y, float z)
{
    translatePoint(&frame->O, x, y, z);
}


