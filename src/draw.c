#include <math.h>
#include <stdio.h>

#include "draw.h"
#include "coord.h"
#include "color.h"
#include "lens.h"
#include "display.h"
#include "pixel.h"

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
    Coord B = *A;
    Color filtered = *color;
    translateCoord(&B, getWidthPosition(l), getHeightPosition(l));
    filterColor(&filtered, getFilter(l));
    pixelDisplay(&B, &filtered);
}

void drawPixel(Lens *l, const Coord *A, float depthA, const Color *color)
{
    int sW = getScreenWidth(l);
    float *zB = getZBuffer(l);
    int i = A->w + A->h * sW;
    if (A->h >= 0 && A->h < getScreenHeight(l) && 
	A->w >= 0 && A->w < sW && 
	(zB[i] < getNearplan(l) || 
	 zB[i] > depthA))
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
    float nearplan = getNearplan(l);

    alpha = (float) (M.w - A->w) / (B->w - A->w);
    depthM = depthA * depthB / ((1 - alpha) * depthB + alpha * depthA);
    if (M.h >= 0 && M.h < sH && M.w >= 0 && M.w < sW && 
	(zB[M.w + M.h * sW] < nearplan || zB[M.w + M.h * sW] > depthM)) {
	translatePixel(l, &M, color);
	zB[M.w + M.h * sW] = depthM;
    }
    
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
		(zB[M.w + M.h * sW] < nearplan || 
		 zB[M.w + M.h * sW] > depthM)) {
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
		(zB[M.w + M.h * sW] < nearplan || 
		 zB[M.w + M.h * sW] > depthM)) {
		translatePixel(l, &M, color);
		//setPixel(M, {255 /depthM , 255 /depthM, 255 /depthM});
		//setPixel(M, {255 * alpha, 0, 0});
		zB[M.w + M.h * sW] = depthM;
	    }
	}
    }
}

void drawTriangle(Lens *l, Texture *triangle, Pixel *A, Pixel *B, Pixel *C)
{
    Coord AB, BC, CA;
    diffCoord(&B->c, &A->c, &AB);
    diffCoord(&C->c, &B->c, &BC);
    diffCoord(&A->c, &C->c, &CA);

    if (productCoord(&AB, &BC) <= 0)
	return;

    float *zB = getZBuffer(l);
    float nearplan = getNearplan(l);
    int sW = getScreenWidth(l);
    int sH = getScreenHeight(l);

    int minW = max(0, min(min(A->c.w, B->c.w), C->c.w));
    int maxW = min(sW - 1, max(max(A->c.w, B->c.w), C->c.w));

    int minH = max(0, min(min(A->c.h, B->c.h), C->c.h));
    int maxH = min(sH - 1, max(max(A->c.h, B->c.h), C->c.h));

    float depthABC = A->depth * B->depth * C->depth;

    float depthAB = A->depth * B->depth;
    float depthBC = B->depth * C->depth;
    float depthCA = C->depth * A->depth;

    Position u, v, w;

    if (triangle) {
	setPosition(&u, A->p.x / A->depth, A->p.y / A->depth);
	setPosition(&v, B->p.x / B->depth, B->p.y / B->depth);
	setPosition(&w, C->p.x / C->depth, C->p.y / C->depth);
    }

    int det = productCoord(&CA, &AB);
    Coord M;

    for (M.h = minH; M.h <= maxH; ++M.h) {
	Coord AM, BM, CM;
	int PAlpha, PBeta, PGamma;
	M.w = minW;

	do {
	    diffCoord(&M, &A->c, &AM);
	    diffCoord(&M, &B->c, &BM);
	    diffCoord(&M, &C->c, &CM);
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

	    float depthM = depthABC / 
		(alpha * depthBC + beta * depthCA + gamma * depthAB);

	    if (zB[M.w + M.h * sW] < nearplan || 
		zB[M.w + M.h * sW] > depthM) {
		Color colorM;
		Color c;
		interpolateColor(&colorM, 
				 &A->light, &B->light, &C->light, 
				 alpha, beta, gamma);
		
		if (triangle) {
		    float denominator = (alpha / A->depth + 
					 beta / B->depth + 
					 gamma / C->depth);
		    Position N;
		    N.x = (alpha * u.x + beta * v.x + gamma * w.x) / 
			denominator;
		    N.y = (alpha * u.y + beta * v.y + gamma * w.y) /
			denominator;

		    loopPosition(&N);
		    getPixelTexture(triangle, &N, &c);
		} else {
		    getUntexturedDisplay(&c);
		}
		productColor(&c, &colorM, &c);
		translatePixel(l, &M, &c);
		zB[M.w + M.h * sW] = depthM;
	    }
	    M.w++;
	    diffCoord(&M, &A->c, &AM);
	    diffCoord(&M, &B->c, &BM);
	    diffCoord(&M, &C->c, &CM);
	}
    }
}
