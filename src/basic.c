#include <SDL/SDL.h>
#include "data.h"
#include "basic.h"
#include "math.h"

void diffRect(const Coord *A, const Coord *B, Coord *S)
{
    setCoord(S,
	     A->w - B->w,
	     A->h - B->h);
}

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

int relativeProduct(const Coord *u, const Coord *v)
{
    return u->h * v->w - u->w * v->h;
}

void vectorProduct(const Point *u, const Point *v, Point *S)
{
    setPoint(S,
	     u->y * v->z - u->z * v->y,
	     u->z * v->x - u->x * v->z,
	     u->x * v->y - u->y * v->x);
}

float distanceCoord(const Coord * A, const Coord * B)
{
    return sqrt((A->w - B->w) * (A->w - B->w) +
		(A->h - B->h) * (A->h - B->h));
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

float loop(float a)
{
    if (a < 0)
	return a - (int) a + 1;
    else if (a >= 1)
	return a - (int) a;
    else
	return a;
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
    vectorProduct(&a, &b, &c);
    normalizePoint(&c, normal);
}

void scalePoint(Point *A, const Point *O, float scale)
{
    setPoint(A,
	     O->x + (A->x - O->x) * scale,
	     O->y + (A->y - O->y) * scale,
	     O->z + (A->z - O->z) * scale);
}

void scaleSolid(Solid *solid, const Point *O, float scale)
{
    int i;
    for (i = 0; i < solid->numVertices; i++)
	scalePoint(&solid->vertices[i], O, scale);
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

void rotSolidXAxis(Solid *solid, const Point *O, float phi)
{
    int i;
    for (i = 0; i < solid->numVertices; i++)
	rotPointXAxis(&solid->vertices[i], O, phi);
}

void rotSolidYAxis(Solid *solid, const Point *O, float rho)
{
    int i;
    for (i = 0; i < solid->numVertices; i++)
	rotPointYAxis(&solid->vertices[i], O, rho);
}

void rotSolidZAxis(Solid *solid, const Point *O, float theta)
{
    int i;
    for (i = 0; i < solid->numVertices; i++)
	rotPointZAxis(&solid->vertices[i], O, theta);
}

void rotVector(const Point *u, const Point *v, float angle, Point *w)
{
    float cosinus = cos(angle), sinus = sin(angle);
    setPoint(w,
	     cosinus * u->x + sinus * (v->y * u->z - v->z * u->y),
	     cosinus * u->y + sinus * (v->z * u->x - v->x * u->z),
	     cosinus * u->z + sinus * (v->x * u->y - v->y * u->x));
}

void direction(float theta, float phi, float rho)
{
    user.j.x = sin(theta);
    user.j.y = cos(theta);
    user.j.z = 0.;

    user.i.x = user.j.y;
    user.i.y = -user.j.x;
    user.i.z = 0.;

    Point a;
    rotVector(&user.j, &user.i, phi, &a);
    user.j = a;
    rotVector(&user.i, &user.j, rho, &a);
    user.i = a;
    vectorProduct(&user.i, &user.j, &user.k);
}

void translatePoint(Point *A, float x, float y, float z)
{
    A->x += x;
    A->y += y;
    A->z += z;
}

void translateSolid(Solid *solid, float x, float y, float z)
{
    int i;
    for (i = 0; i < solid->numVertices; i++)
	translatePoint(&solid->vertices[i], x, y, z);
}

void setPixel(const Coord *A, const Color *color)
{
    Uint32 pixel = SDL_MapRGB(screen->format,
			      color->r, color->g, color->b);
    *((Uint32 *) (screen->pixels) + A->w + A->h * screen->w) = pixel;
}

void drawPixel(const Coord *A, const Color *color)
{
    if (A->h >= 0 && A->h < HEIGHT && A->w >= 0 && A->w < WIDTH)
	setPixel(A, color);
}
