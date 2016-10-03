#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "point.h"


void sumPoint(const Point *A, const Point *B, Point *S)
{
    setPoint(S,
             A->x + B->x,
             A->y + B->y,
             A->z + B->z);
}

void diffPoint(const Point *A, const Point *B, Point *S)
{
    setPoint(S,
             A->x - B->x,
             A->y - B->y,
             A->z - B->z);
}

float scalarProduct(const Point *u, const Point *v)
{
    return u->x * v->x + u->y * v->y + u->z * v->z;
}

void pointProduct(const Point *u, const Point *v, Point *S)
{
    setPoint(S,
             u->y * v->z - u->z * v->y,
             u->z * v->x - u->x * v->z,
             u->x * v->y - u->y * v->x);
}
float distancePoint(const Point *u, const Point *v)
{
    return sqrt((u->x - v->x) * (u->x - v->x) +
                (u->y - v->y) * (u->y - v->y) +
                (u->z - v->z) * (u->z - v->z));
}

float normPoint(const Point *u)
{
    return sqrt((u->x) * (u->x) + (u->y) * (u->y) + (u->z) * (u->z));
}

void normalizePoint(const Point *u, Point *S)
{
    float n = normPoint(u);
    setPoint(S,
             u->x / n,
             u->y / n,
             u->z / n);
}

void normal(const Point *A, const Point *B, const Point *C,
            Point *normal)
{
    Point a, b, c;
    diffPoint(B, A, &a);
    diffPoint(C, A, &b);
    pointProduct(&a, &b, &c);
    normalizePoint(&c, normal);
}

void scalePoint(Point *A, const Point *O, float scale)
{
    setPoint(A,
             O->x + (A->x - O->x) * scale,
             O->y + (A->y - O->y) * scale,
             O->z + (A->z - O->z) * scale);
}

void rotPointXAxis(Point *A, const Point *O, float phi)
{
    Point OA;
    diffPoint(A, O, &OA);
    setPoint(A,
             A->x,
             OA.y * cos(phi) - OA.z * sin(phi) + O->y,
             OA.z * cos(phi) + OA.y * sin(phi) + O->z);
}

void rotPointYAxis(Point *A, const Point *O, float rho)
{
    Point OA;
    diffPoint(A, O, &OA);
    setPoint(A,
             OA.x * cos(rho) - OA.z * sin(rho) + O->x,
             A->y,
             OA.z * cos(rho) + OA.x * sin(rho) + O->z);
}

void rotPointZAxis(Point *A, const Point *O, float theta)
{
    Point OA;
    diffPoint(A, O, &OA);
    setPoint(A,
             OA.x * cos(theta) - OA.y * sin(theta) + O->x,
             OA.y * cos(theta) + OA.x * sin(theta) + O->y,
             A->z);
}

void rotPoint(const Point *u, const Point *v, float angle, Point *w)
{
    float cosinus = cos(angle), sinus = sin(angle);
    setPoint(w,
             cosinus * u->x + sinus * (v->y * u->z - v->z * u->y),
             cosinus * u->y + sinus * (v->z * u->x - v->x * u->z),
             cosinus * u->z + sinus * (v->x * u->y - v->y * u->x));
}

void translatePoint(Point *A, float x, float y, float z)
{
    A->x += x;
    A->y += y;
    A->z += z;
}

void setPoint(Point *p, float x, float y, float z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}
