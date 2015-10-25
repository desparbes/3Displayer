#include <math.h>

#include "point.h"
#include "frame.h"

void initFrame(Frame *f)
{
    setPoint(&f->O, 0., 0., 0.);
    setPoint(&f->i, 1., 0., 0.);
    setPoint(&f->j, 0., 1., 0.);
    setPoint(&f->k, 0., 0., 1.);
}    

void rotateFrameAroundPoint(Frame *f, const Point *axis, float angle)
{
    rotPoint(&f->i, axis, angle, &f->i);
    rotPoint(&f->j, axis, angle, &f->j);
    rotPoint(&f->k, axis, angle, &f->k);
}    

void rotateFrame(Frame *frame, float theta, float phi, float rho)
{
    setPoint(&frame->j, sin(theta), cos(theta), 0.);
    setPoint(&frame->i, frame->j.y, -frame->j.x, 0.);
    rotPoint(&frame->j, &frame->i, phi, &frame->j);
    rotPoint(&frame->i, &frame->j, rho, &frame->i);
    pointProduct(&frame->i, &frame->j, &frame->k);
}

void translateFrame(Frame *frame, Point *A, float scale)
{
    translatePoint(&frame->O, scale * A->x, scale * A->y, scale * A->z);
}

void getAbsolutePointFromFrame(const Frame *f, const Point *A, Point *B)
{
    setPoint(B, 
	     f->O.x + A->x * f->i.x + A->y * f->j.x + A->z * f->k.x,
	     f->O.y + A->x * f->i.y + A->y * f->j.y + A->z * f->k.y,
	     f->O.z + A->x * f->i.z + A->y * f->j.z + A->z * f->k.z);
}



