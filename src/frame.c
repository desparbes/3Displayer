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
    setPoint(&frame->j, sin(theta), cos(theta), 0.);
    setPoint(&frame->i, frame->j.y, -frame->j.x, 0.);
    rotPoint(&frame->j, &frame->i, phi, &frame->j);
    rotPoint(&frame->i, &frame->j, rho, &frame->i);
    pointProduct(&frame->i, &frame->j, &frame->k);
}

void translateFrame(Frame *frame, float x, float y, float z)
{
    translatePoint(&frame->O, x, y, z);
}

void absolutePointInFrame(const Frame *f, const Point *A, Point *B)
{
    setPoint(B, 
	     f->O.x + A->x * f->i.x + A->y * f->j.x + A->z * f->k.x,
	     f->O.y + A->x * f->i.y + A->y * f->j.y + A->z * f->k.y,
	     f->O.z + A->x * f->i.z + A->y * f->j.z + A->z * f->k.z);
}    



