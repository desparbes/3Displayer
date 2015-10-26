#include <math.h>

#include "draw.h"
#include "coord.h"
#include "color.h"
#include "lens.h"
#include "display.h"
#include "scene.h"

static inline int min(int a, int b)
{
    return (a < b) ? a : b;
}

static inline int max(int a, int b)
{
    return (a > b) ? a : b;
}

static void translatePixel(Lens *l, const Coord *A, const Color *color)
{
    Coord B;
    Color filtered = *color;
    sumCoord(A, getScreenPosition(l), &B);
    filterColor(&filtered, getFilter(l));
    pixelDisplay(&B, &filtered);
}

void drawPixel(Lens *l, const Coord *A, float depthA, const Color *color)
{
    int i = A->w + A->h * getScreenWidth(l);
    if (A->h >= 0 && A->h < getScreenHeight(l) && 
	A->w >= 0 && A->w < getScreenWidth(l) && 
	(getZBuffer(l)[i] < 0 || 
	 getZBuffer(l)[i] > depthA))
	translatePixel(l, A, color);
}

void drawSegment(Lens *l, const Coord *A, const Coord *B,
		 float depthA, float depthB,
		 const Color *color)
{
    Coord M = *A;
    int dx = B->w - A->w;
    int dy = B->h - A->h;
    int xIncr = dx > 0 ? 1 : -1;
    int yIncr = dy > 0 ? 1 : -1;
    float alpha, depthM;
    int error;
    float *zB = getZBuffer(l);
    int sW = getScreenWidth(l);
    int sH = getScreenHeight(l);

    if ((abs(dx) > abs(dy))) {
	error = dx;
	dx = 2 * dx;
	dy = 2 * dy;
	while (M.w != B->w) {
	    M.w += xIncr;
	    alpha = (float) (M.w - A->w) / (B->w - A->w);
	    depthM = depthA * depthB / ((1 - alpha) * depthB + alpha * depthA);
	    if ((error -= yIncr * dy) <= 0) {
		M.h += yIncr;
		error += xIncr * dx;
	    }
	    if (M.h >= 0 && M.h < sH && M.w >= 0 && M.w < sW && 
		(zB[M.w + M.h * sW] < 0 || zB[M.w + M.h * sW] > depthM)) {
		translatePixel(l, &M, color);
		//setPixel(M, {255 /depthM , 255 /depthM, 255 /depthM});
		//setPixel(M, {255 * alpha, 0, 0});
		zB[M.w + M.h * sW] = depthM;
	    }
	}
    } else {
	error = dy;
	dy = 2 * dy;
	dx = 2 * dx;
	while (M.h != B->h) {
	    M.h += yIncr;
	    alpha = (float) (M.h - A->h) / (B->h - A->h);
	    depthM = depthA * depthB / ((1 - alpha) * depthB + alpha * depthA);
	    if ((error -= xIncr * dx) <= 0) {
		M.w += xIncr;
		error += yIncr * dy;
	    }
	    if (M.h >= 0 && M.h < sH && M.w >= 0 && M.w < sW && 
		(zB[M.w + M.h * sW] < 0 || zB[M.w + M.h * sW] > depthM)) {
		translatePixel(l, &M, color);
		//setPixel(M, {255 /depthM , 255 /depthM, 255 /depthM});
		//setPixel(M, {255 * alpha, 0, 0});
		zB[M.w + M.h * sW] = depthM;
	    }
	}
    }
}

void drawTriangle(Lens *l, const Coord *A, const Coord *B, const Coord *C,
		  float depthA, float depthB, float depthC,
		  Texture *triangle,
		  const Position *U, const Position *V, const Position *W,
		  const Point *normalA,
		  const Point *normalB,
		  const Point *normalC)
{
    Coord AB, BC, CA;
    diffCoord(B, A, &AB);
    diffCoord(C, B, &BC);
    diffCoord(A, C, &CA);

    if (productCoord(&AB, &BC) <= 0)
	return;

    float *zB = getZBuffer(l);
    int sW = getScreenWidth(l);
    int sH = getScreenHeight(l);

    int minW = min(min(A->w, B->w), C->w);
    int maxW = max(max(A->w, B->w), C->w);

    int minH = min(min(A->h, B->h), C->h);
    int maxH = max(max(A->h, B->h), C->h);

    if (minW < 0)
	minW = 0;
    if (maxW >= sW)
	maxW = sW - 1;
    if (minH < 0)
	minH = 0;
    if (maxH >= sH)
	maxH = sH - 1;

    Point p;
    sumPoint(normalA, getLight(), &p);
    float scaleA = 1 - normPoint(&p) / 2;
    sumPoint(normalB, getLight(), &p);
    float scaleB = 1 - normPoint(&p) / 2;
    sumPoint(normalC, getLight(), &p);
    float scaleC = 1 - normPoint(&p) / 2;

    float scaleABC = scaleA * scaleB * scaleC;
    
    float scaleAB = scaleA * scaleB;
    float scaleBC = scaleB * scaleC;
    float scaleCA = scaleC * scaleA;

    float depthABC = depthA * depthB * depthC;

    float depthAB = depthA * depthB;
    float depthBC = depthB * depthC;
    float depthCA = depthC * depthA;

    float scale;
    float depthM;

    Position N, u, v, w;
    if (triangle != NULL) {
	setPosition(&u,
		   U->x / depthA,
		   U->y / depthA);
	setPosition(&v,
	           V->x / depthB,
		   V->y / depthB);
	setPosition(&w,
		   W->x / depthC,
		   W->y / depthC);
    }

    int det = productCoord(&CA, &AB);

    Color c;

    Coord M;
    Coord AM, BM, CM;
    int PAlpha, PBeta, PGamma;

    for (M.h = minH; M.h <= maxH; ++M.h) {
	M.w = minW;

	do {
	    diffCoord(&M, A, &AM);
	    diffCoord(&M, B, &BM);
	    diffCoord(&M, C, &CM);
	    M.w++;
	} while (M.w <= maxW && 
		 (productCoord(&AB, &AM) < 0 ||
		  productCoord(&BC, &BM) < 0 || 
		  productCoord(&CA, &CM) < 0));
	M.w--;
	while (M.w <= maxW && 
	       (PAlpha = productCoord(&BC, &BM)) >= 0 && 
	       (PBeta = productCoord(&CA, &CM)) >= 0 && 
	       (PGamma = productCoord(&AB, &AM)) >= 0) {

	    float alpha = (float) PAlpha / (float) det;
	    float beta = (float) PBeta / (float) det;
	    float gamma = (float) PGamma / (float) det;

	    depthM = depthABC / 
		(alpha * depthBC + beta * depthCA + gamma * depthAB);

	    if (zB[M.w + M.h * sW] < 0
		|| zB[M.w + M.h * sW] > depthM) {
		scale = scaleABC / 
		    (alpha * scaleBC + beta * scaleCA + gamma * scaleAB);
		
		if (triangle) {
		    float denominator = (alpha / depthA + 
					 beta / depthB + 
					 gamma / depthC);
		    N.x = (alpha * u.x + beta * v.x + gamma * w.x) / 
			denominator;
		    N.y = (alpha * u.y + beta * v.y + gamma * w.y) /
			denominator;

		    loopPosition(&N);
		    getPixelTexture(triangle, &N, &c);
		} else {
		    c.r = 255;
		    c.g = 0;
		    c.b = 255;
		}
		scaleColor(&c, scale);
		translatePixel(l, &M, &c);
		zB[M.w + M.h * sW] = depthM;
	    }
	    M.w++;
	    diffCoord(&M, A, &AM);
	    diffCoord(&M, B, &BM);
	    diffCoord(&M, C, &CM);
	}
    }
}
