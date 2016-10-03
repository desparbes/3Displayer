#ifndef POINT_H
#define POINT_H

typedef struct Point {
    float x;
    float y;
    float z;
} Point;

void setPoint(Point *p, float x, float y, float z);
void sumPoint(const Point *A, const Point *B, Point *S);
void diffPoint(const Point *A, const Point *B, Point *S);
float scalarProduct(const Point *u, const Point *v);
void pointProduct(const Point *u, const Point *v, Point *S);
float distancePoint(const Point *u, const Point *v);
float normPoint(const Point *u);
void normalizePoint(const Point *u, Point *S);
void normal(const Point *A, const Point *B, const Point *C,
            Point *normal);
void scalePoint(Point *A, const Point *O, float scale);
void rotPointXAxis(Point *A, const Point *O, float phi);
void rotPointYAxis(Point *A, const Point *O, float rho);
void rotPointZAxis(Point *A, const Point *O, float theta);
void rotPoint(const Point *u, const Point *v, float angle, Point *w);
void translatePoint(Point *A, float x, float y, float z);

#endif // POINT_H
