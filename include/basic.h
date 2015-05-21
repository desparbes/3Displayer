#ifndef BASIC_H
#define BASIC_H


#include "data.h"

void diffRect(const Coord *A, const Coord *B, Coord *S);
void sumPoint(const Point *A, const Point *B, Point *S);
void diffPoint(const Point *A, const Point *B, Point *S);

float scalarProduct(const Point *u, const Point *v);
int relativeProduct(const Coord *u, const Coord *v);
void vectorProduct(const Point *u, const Point *v, Point *S);

float distanceCoord(const Coord * A, const Coord * B);
float distancePoint(const Point *u, const Point *v);
float normPoint(const Point *u);

float loop(float a);

void normalizePoint(const Point *u, Point *S);
void normal(const Point *A, const Point *B, const Point *C,
	    Point *normal);

void scalePoint(Point *A, const Point *O, float scale);
void scaleSolid(Solid *solid, const Point *O, float scale);

void rotPointXAxis(Point *A, const Point *O, float phi);
void rotPointYAxis(Point *A, const Point *O, float rho);
void rotPointZAxis(Point *A, const Point *O, float theta);

void rotSolidXAxis(Solid *solid, const Point *O, float phi);
void rotSolidYAxis(Solid *solid, const Point *O, float rho);
void rotSolidZAxis(Solid *solid, const Point *O, float theta);

void rotVector(const Point *u, const Point *v, float angle, Point *w);
void direction(float theta, float phi, float rho);

void translatePoint(Point *A, float x, float y, float z);
void translateSolid(Solid *solid, float x, float y, float z);

void setPixel(const Coord *A, const Color *color);
void drawPixel(const Coord *A, const Color *color);

#endif //BASIC_H
