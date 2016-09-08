#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "draw.h"
#include "coord.h"
#include "color.h"
#include "lens.h"
#include "pixel.h"
#include "window.h"

typedef struct Renderer Renderer;
struct Renderer {
    Color untextured;
};

static Renderer renderer;

static void translatePixel(Lens *l, Coord A, Color filtered)
{
    Color filter = getFilter(l);
    
    TRANSLATE_COORD(A, getWidthPosition(l), getHeightPosition(l));
    FILTER_COLOR(filtered, filtered, filter);
    
    if (getOverlapping(l)) {
	Color back;
	back = getPixelWindow(A);
	AVERAGE_COLOR(filtered, filtered, back, 128);
    }
    setPixelWindow(A, filtered);
}

void drawPixel(Lens *l, Coord A, float depthA, Color color)
{
    int sW = getScreenWidth(l);
    float *zB = getZBuffer(l);

    int i = A.w + A.h * sW;
    if (A.h >= 0 && A.h < getScreenHeight(l) && 
	A.w >= 0 && A.w < sW &&
        (zB[i] < getNearplan(l) || zB[i] > depthA))
    {
	translatePixel(l, A, color);
    }
}

void drawSegment(Lens *l, Coord A, Coord B,
		 float depthA, float depthB, Color color)
{
    Coord M = A;
    int dx = B.w - A.w;
    int dy = B.h - A.h;
    int xIncr = dx > 0 ? 1 : -1;
    int yIncr = dy > 0 ? 1 : -1;
    float alpha, depthM;
    int error;
    float *zB = getZBuffer(l);
    int sW = getScreenWidth(l);
    int sH = getScreenHeight(l);
    float nearplan = getNearplan(l);

    alpha = (float) (M.w - A.w) / (B.w - A.w);
    depthM = depthA * depthB / ((1 - alpha) * depthB + alpha * depthA);
    if (M.h >= 0 && M.h < sH && M.w >= 0 && M.w < sW && 
	(zB[M.w + M.h * sW] < nearplan || zB[M.w + M.h * sW] > depthM)) {
	translatePixel(l, M, color);
	zB[M.w + M.h * sW] = depthM;
    }
    
    if ((abs(dx) > abs(dy))) {
	error = dx;
	dx = 2 * dx;
	dy = 2 * dy;
	while (M.w != B.w) {
	    M.w += xIncr;
	    alpha = (float) (M.w - A.w) / (B.w - A.w);
	    depthM = depthA * depthB / ((1 - alpha) * depthB + alpha * depthA);
	    if ((error -= yIncr * dy) <= 0) {
		M.h += yIncr;
		error += xIncr * dx;
	    }
	    if (M.h >= 0 && M.h < sH && M.w >= 0 && M.w < sW && 
		(zB[M.w + M.h * sW] < nearplan || 
		 zB[M.w + M.h * sW] > depthM)) {
		translatePixel(l, M, color);
		//setPixel(M, {255 /depthM , 255 /depthM, 255 /depthM});
		//setPixel(M, {255 * alpha, 0, 0});
		zB[M.w + M.h * sW] = depthM;
	    }
	}
    } else {
	error = dy;
	dy = 2 * dy;
	dx = 2 * dx;
	while (M.h != B.h) {
	    M.h += yIncr;
	    alpha = (float) (M.h - A.h) / (B.h - A.h);
	    depthM = depthA * depthB / ((1 - alpha) * depthB + alpha * depthA);
	    if ((error -= xIncr * dx) <= 0) {
		M.w += xIncr;
		error += yIncr * dy;
	    }
	    if (M.h >= 0 && M.h < sH && M.w >= 0 && M.w < sW && 
		(zB[M.w + M.h * sW] < nearplan || 
		 zB[M.w + M.h * sW] > depthM)) {
		translatePixel(l, M, color);
		//setPixel(M, {255 /depthM , 255 /depthM, 255 /depthM});
		//setPixel(M, {255 * alpha, 0, 0});
		zB[M.w + M.h * sW] = depthM;
	    }
	}
    }
}

void drawTriangle(Lens *l, Texture *triangle, Pixel A, Pixel B, Pixel C)
{
    Coord AB, BC, CA;
    DIFF_COORD(AB, B.coord, A.coord);
    DIFF_COORD(BC, C.coord, B.coord);
    DIFF_COORD(CA, A.coord, C.coord);

    if (PRODUCT_COORD(AB, BC) <= 0)
	return;

    float *zB = getZBuffer(l);
    float nearplan = getNearplan(l);
    int sW = getScreenWidth(l);
    int sH = getScreenHeight(l);

    int minW = MAX(0, MIN(MIN(A.coord.w, B.coord.w), C.coord.w));
    int maxW = MIN(sW - 1, MAX(MAX(A.coord.w, B.coord.w), C.coord.w));

    int minH = MAX(0, MIN(MIN(A.coord.h, B.coord.h), C.coord.h));
    int maxH = MIN(sH - 1, MAX(MAX(A.coord.h, B.coord.h), C.coord.h));

    float depthABC = A.depth * B.depth * C.depth;

    float depthAB = A.depth * B.depth;
    float depthBC = B.depth * C.depth;
    float depthCA = C.depth * A.depth;

    Position u, v, w;

    if (triangle) {
	setPosition(&u, A.p.x / A.depth, A.p.y / A.depth);
	setPosition(&v, B.p.x / B.depth, B.p.y / B.depth);
	setPosition(&w, C.p.x / C.depth, C.p.y / C.depth);
    }

    int det = PRODUCT_COORD(CA, AB);
    Coord M;

    for (M.h = minH; M.h <= maxH; ++M.h) {
	Coord AM, BM, CM;
	int PAlpha, PBeta, PGamma;
	M.w = minW;

	do {
	    DIFF_COORD(AM, M, A.coord);
	    DIFF_COORD(BM, M, B.coord);
	    DIFF_COORD(CM, M, C.coord);
	    M.w++;
	} while (M.w <= maxW && 
		 (PRODUCT_COORD(AB, AM) < 0 ||
		  PRODUCT_COORD(BC, BM) < 0 || 
		  PRODUCT_COORD(CA, CM) < 0));
	M.w--;
	while (M.w <= maxW && 
	       (PAlpha = PRODUCT_COORD(BC, BM)) >= 0 && 
	       (PBeta = PRODUCT_COORD(CA, CM)) >= 0 && 
	       (PGamma = PRODUCT_COORD(AB, AM)) >= 0) {

	    float alpha = (float) PAlpha / (float) det;
	    float beta = (float) PBeta / (float) det;
	    float gamma = (float) PGamma / (float) det;

	    float depthM = depthABC / 
		(alpha * depthBC + beta * depthCA + gamma * depthAB);

	    if (zB[M.w + M.h * sW] < nearplan || 
		zB[M.w + M.h * sW] > depthM) {
		Color colorM;
		Color c;
		INTERPOLATE_COLOR(colorM, 
                                  A.light, B.light, C.light, 
                                  alpha, beta, gamma);
		
		if (triangle) {
		    float denominator = (alpha / A.depth + 
					 beta / B.depth + 
					 gamma / C.depth);
		    Position N;
		    N.x = (alpha * u.x + beta * v.x + gamma * w.x) / 
			denominator;
		    N.y = (alpha * u.y + beta * v.y + gamma * w.y) /
			denominator;

		    loopPosition(&N);
		    getPixelTexture(triangle, &N, &c);
		} else {
		    c = renderer.untextured;
		}
		PRODUCT_COLOR(c, colorM, c);
		translatePixel(l, M, c);
		zB[M.w + M.h * sW] = depthM;
	    }
	    M.w++;
	    DIFF_COORD(AM, M, A.coord);
	    DIFF_COORD(BM, M, B.coord);
	    DIFF_COORD(CM, M, C.coord);
	}
    }
}
