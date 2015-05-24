#include <math.h>

#include "project.h"
#include "point.h"
#include "coord.h"
#include "draw.h"
#include "color.h"
#include "scene.h"

// return the vector between camera.O and the intersection of
// (AB) and the NEARPLAN
void projectPoint(const Point *A, const Point *B, Point *S)
{
    Point AB, d;
    diffPoint(B, A, &AB);
    diffPoint(A, &getCamera()->O, &d);
    float k =
	(getNearplan() - scalarProduct(&getCamera()->j, &d))
	/ scalarProduct(&getCamera()->j, &AB);

    setPoint(S,
	     A->x + k * AB.x - getCamera()->O.x,
	     A->y + k * AB.y - getCamera()->O.y,
	     A->z + k * AB.z - getCamera()->O.z);
}

// return the Coord projection of a vector between scene.camera.O
// and A, knowing the depth of A
void projectCoord(const Point *OA, float depth, Coord *S)
{
    S->w = getScreenWidth() / 
	(2. * tan(getWfov() * M_PI / 360.)) * 
	scalarProduct(&getCamera()->i, OA) / 
	depth + getScreenWidth() / 2;
    S->h = -getScreenHeight() / 
	(2. * tan(getHfov() * M_PI / 360.)) * 
	scalarProduct(&getCamera()->k, OA) / 
	depth + getScreenHeight() / 2;
}

void projectVertex(const Point *A, const Color *color)
{
    Point OA;
    diffPoint(A, &getCamera()->O, &OA);
    float depthA = scalarProduct(&getCamera()->j, &OA);

    Coord t;
    projectCoord(&OA, depthA, &t);
    if (depthA > getNearplan())
	drawPixel(&t, depthA, color);
}

void projectSegment(const Point *A, const Point *B, const Color *color)
{
    Point OA, OB, AB;
    diffPoint(A, &getCamera()->O, &OA);
    diffPoint(B, &getCamera()->O, &OB);
    float depthA = scalarProduct(&getCamera()->j, &OA);
    float depthB = scalarProduct(&getCamera()->j, &OB);

    Coord t, u;
    if (depthA > getNearplan() && depthB > getNearplan()) {
	projectCoord(&OA, depthA, &t);
	projectCoord(&OB, depthB, &u);
	drawSegment(&t, &u, depthA, depthB, color);
    } else if (depthA < getNearplan() && depthB > getNearplan()) {
	projectPoint(A, B, &AB);
	projectCoord(&AB, getNearplan(), &t);
	projectCoord(&OB, depthB, &u);
    } else if (depthA > getNearplan() && depthB < getNearplan()) {
	projectPoint(A, B, &AB);
	projectCoord(&OA, depthA, &t);
	projectCoord(&AB, getNearplan(), &u);
    }
    drawSegment(&t, &u, depthA, depthB, color);
}

void projectTriangle(const Point *A, const Point *B, const Point *C,
		     SDL_Surface *triangle,
		     const Texture *U, const Texture *V, const Texture *W,
		     const Point *normalA, const Point *normalB,
		     const Point *normalC)
{
    Point OA, OB, OC;
    diffPoint(A, &getCamera()->O, &OA);
    diffPoint(B, &getCamera()->O, &OB);
    diffPoint(C, &getCamera()->O, &OC);
    float depthA = scalarProduct(&getCamera()->j, &OA);
    float depthB = scalarProduct(&getCamera()->j, &OB);
    float depthC = scalarProduct(&getCamera()->j, &OC);
    int test = (depthA > getNearplan()) + 
	       (depthB > getNearplan()) + 
	       (depthC > getNearplan());

    if (test == 3) {
	Coord a, b, c;
	projectCoord(&OA, depthA, &a);
	projectCoord(&OB, depthB, &b);
	projectCoord(&OC, depthC, &c);
	drawTriangle(&a, &b, &c,
		     depthA, depthB, depthC,
		     triangle,
		     U, V, W,
		     normalA, normalB, normalC);
    } else if (test == 0) {
	return;
    } else if (test == 1) {
	if (depthA > getNearplan()) {
	    Point b, c;
	    diffPoint(B, A, &b);
	    diffPoint(C, A, &c);

	    float kB = (getNearplan() - depthA) / 
		scalarProduct(&getCamera()->j, B);
	    float kC = (getNearplan() - depthA) / 
		scalarProduct(&getCamera()->j, C);

	    setPoint(&OB, 
		     A->x + kB * b.x - getCamera()->O.x,
		     A->y + kB * b.y - getCamera()->O.y,
		     A->z + kB * b.z - getCamera()->O.z);
	    setPoint(&OC,
		     A->x + kC * c.x - getCamera()->O.x,
		     A->y + kC * c.y - getCamera()->O.y,
		     A->z + kC * c.z - getCamera()->O.z);

	    Coord a, bn, cn;
	    projectCoord(&OA, depthA, &a);
	    projectCoord(&OB, getNearplan(), &bn);
	    projectCoord(&OC, getNearplan(), &cn);

	    Texture UV, UW;
	    setTexture(&UV,
		       (V->x - U->x) * kB + U->x,
		       (V->y - U->y) * kB + U->y);
	    setTexture(&UW,
		       (W->x - U->x) * kC + U->x,
		       (W->y - U->y) * kC + U->y);

	    Point nAB, nAC;
	    setPoint(&nAB,
		     (normalB->x - normalA->x) * kB + normalA->x,
		     (normalB->y - normalA->y) * kB + normalA->y,
		     (normalB->z - normalA->z) * kB + normalA->z);
	    setPoint(&nAC,
		     (normalC->x - normalA->x) * kC + normalA->x,
		     (normalC->y - normalA->y) * kC + normalA->y,
		     (normalC->z - normalA->z) * kC + normalA->z);
	    
	    drawTriangle(&a, &bn, &cn,
			 depthA,
			 (depthB - depthA) * kB + depthA,
			 (depthC - depthA) * kC + depthA,
			 triangle,
			 U, &UV, &UW,
			 normalA, &nAB, &nAC);
	} else if (depthB > getNearplan()) {
	    Point c, a;
	    diffPoint(C, B, &c);
	    diffPoint(A, B, &a);

	    float kC = (getNearplan() - depthB) / 
		scalarProduct(&getCamera()->j, &c);
	    float kA = (getNearplan() - depthB) / 
		scalarProduct(&getCamera()->j, &a);

	    setPoint(&OC,
		     B->x + kC * c.x - getCamera()->O.x,
		     B->y + kC * c.y - getCamera()->O.y,
		     B->z + kC * c.z - getCamera()->O.z);
	    setPoint(&OA,
		     B->x + kA * a.x - getCamera()->O.x,
		     B->y + kA * a.y - getCamera()->O.y,
		     B->z + kA * a.z - getCamera()->O.z);

	    Coord b, cn, an;
	    projectCoord(&OB, depthB, &b);
	    projectCoord(&OC, getNearplan(), &cn);
	    projectCoord(&OA, getNearplan(), &an);

	    Texture VW, VU;
	    setTexture(&VW,
		       (W->x - V->x) * kC + V->x,
		       (W->y - V->y) * kC + V->y);
	    setTexture(&VU,
		       (U->x - V->x) * kA + V->x,
		       (U->y - V->y) * kA + V->y);

	    Point nBC, nBA;
	    setPoint(&nBC,
		     (normalC->x - normalB->x) * kC + normalB->x,
		     (normalC->y - normalB->y) * kC + normalB->y,
		     (normalC->z - normalB->z) * kC + normalB->z);
	    setPoint(&nBA,
		     (normalA->x - normalB->x) * kA + normalB->x,
		     (normalA->y - normalB->y) * kA + normalB->y,
		     (normalA->z - normalB->z) * kA + normalB->z);
	    
	    drawTriangle(&b, &cn, &an,
			 depthB,
			 (depthC - depthB) * kC + depthB,
			 (depthA - depthB) * kA + depthB,
			 triangle,
			 V, &VW, &VU,
			 normalB, &nBC, &nBA);

	} else if (depthC > getNearplan()) {
	    Point a,b;
	    diffPoint(A, C, &a);
	    diffPoint(B, C, &b);

	    float kA = (getNearplan() - depthC) / 
		scalarProduct(&getCamera()->j, &a);
	    float kB = (getNearplan() - depthC) / 
		scalarProduct(&getCamera()->j, &b);

	    setPoint(&OA,
		     C->x + kA * a.x - getCamera()->O.x,
		     C->y + kA * a.y - getCamera()->O.y,
		     C->z + kA * a.z - getCamera()->O.z);
	    setPoint(&OB,
		     C->x + kB * b.x - getCamera()->O.x,
		     C->y + kB * b.y - getCamera()->O.y,
		     C->z + kB * b.z - getCamera()->O.z);

	    Coord c, an, bn;
	    projectCoord(&OC, depthC, &c);
	    projectCoord(&OA, getNearplan(), &an);
	    projectCoord(&OB, getNearplan(), &bn);

	    Texture WU, WV;
	    setTexture(&WU,
		       (U->x - W->x) * kA + W->x,
		       (U->y - W->y) * kA + W->y);
	    setTexture(&WV,
		       (V->x - W->x) * kB + W->x,
		       (V->y - W->y) * kB + W->y);

	    Point nCA, nCB;
	    setPoint(&nCA,
		     (normalA->x - normalC->x) * kA + normalC->x,
		     (normalA->y - normalC->y) * kA + normalC->y,
		     (normalA->z - normalC->z) * kA + normalC->z);
	    setPoint(&nCB,
		     (normalB->x - normalC->x) * kB + normalC->x,
		     (normalB->y - normalC->y) * kB + normalC->y,
		     (normalB->z - normalC->z) * kB + normalC->z);
	    
	    drawTriangle(&c, &an, &bn,
			 depthC,
			 (depthA - depthC) * kA + depthC,
			 (depthB - depthC) * kB + depthC,
			 triangle,
			 W, &WU, &WV,
			 normalC, &nCA, &nCB);
	}
    } else {
	// Coord XY, XZ, Y, Z;
	if (depthA < getNearplan()) {
	    Point AB, AC;
	    diffPoint(B, A, &AB);
	    diffPoint(C, A, &AC);

	    float kB = (getNearplan() - depthA) / 
		scalarProduct(&getCamera()->j, &AB);
	    float kC = (getNearplan() - depthA) / 
		scalarProduct(&getCamera()->j, &AC);

	    Point OpB, OpC;
	    setPoint(&OpB,
		     A->x + kB * AB.x - getCamera()->O.x,
		     A->y + kB * AB.y - getCamera()->O.y,
		     A->z + kB * AB.z - getCamera()->O.z);
	    setPoint(&OpC,
		     A->x + kC * AC.x - getCamera()->O.x,
		     A->y + kC * AC.y - getCamera()->O.y,
		     A->z + kC * AC.z - getCamera()->O.z);

	    Coord opbn, c, opcn;
	    projectCoord(&OpB, getNearplan(), &opbn);
	    projectCoord(&OC, depthC, &c);
	    projectCoord(&OpC, getNearplan(), &opcn);

	    Texture VU, WU;
	    setTexture(&VU,
		       (V->x - U->x) * kB + U->x,
		       (V->y - U->y) * kB + U->y);
	    setTexture(&WU,
		       (W->x - U->x) * kC + U->x,
		       (W->y - U->y) * kC + U->y);

	    Point nAB, nAC;
	    setPoint(&nAB,
		     (normalB->x - normalA->x) * kB + normalA->x,
		     (normalB->y - normalA->y) * kB + normalA->y,
		     (normalB->z - normalA->z) * kB + normalA->z);
	    setPoint(&nAC,
		     (normalC->x - normalA->x) * kC + normalA->x,
		     (normalC->y - normalA->y) * kC + normalA->y,
		     (normalC->z - normalA->z) * kC + normalA->z);
	    
	    drawTriangle(&opbn, &c, &opcn,
			 (depthB - depthA) * kB + depthA,
			 depthC,
			 (depthC - depthA) * kC + depthA,
			 triangle,
			 &VU, W, &WU,
			 &nAB, normalC, &nAC);

	    Coord b;
	    projectCoord(&OB, depthB, &b);
	    drawTriangle(&opbn, &b, &c,
			 (depthB - depthA) * kB + depthA,
			 depthB, depthC,
			 triangle,
			 &VU, V, W,
			 &nAB, normalB, normalC);
	} else if (depthB < getNearplan()) {
	    Point BC, BA;
	    diffPoint(C, B, &BC);
	    diffPoint(A, B, &BA);

	    float kC = (getNearplan() - depthB) / 
		scalarProduct(&getCamera()->j, &BC);
	    float kA = (getNearplan() - depthB) / 
		scalarProduct(&getCamera()->j, &BA);

	    Point OpC, OpA;
	    setPoint(&OpC,
		     B->x + kC * BC.x - getCamera()->O.x,
		     B->y + kC * BC.y - getCamera()->O.y,
		     B->z + kC * BC.z - getCamera()->O.z);
	    setPoint(&OpA,
		     B->x + kA * BA.x - getCamera()->O.x,
		     B->y + kA * BA.y - getCamera()->O.y,
		     B->z + kA * BA.z - getCamera()->O.z);

	    Coord opcn, a, opan;
	    projectCoord(&OpC, getNearplan(), &opcn);
	    projectCoord(&OA, depthA, &a);
	    projectCoord(&OpA, getNearplan(), &opan);
	    Texture VW, VU;
	    setTexture(&VW,
		       (W->x - V->x) * kC + V->x,
		       (W->y - V->y) * kC + V->y);
	    setTexture(&VU,
		       (U->x - V->x) * kA + V->x,
		       (U->y - V->y) * kA + V->y);

	    Point nBC, nBA;
	    setPoint(&nBC,
		     (normalC->x - normalB->x) * kC + normalB->x,
		     (normalC->y - normalB->y) * kC + normalB->y,
		     (normalC->z - normalB->z) * kC + normalB->z);
	    setPoint(&nBA,
		     (normalA->x - normalB->x) * kA + normalB->x,
		     (normalA->y - normalB->y) * kA + normalB->y,
		     (normalA->z - normalB->z) * kA + normalB->z);
	    drawTriangle(&opcn, &a, &opan,
			 (depthC - depthB) * kC + depthB,
			 depthA,
			 (depthA - depthB) * kA + depthB,
			 triangle,
			 &VW, U, &VU,
			 &nBC, normalA, &nBA);

	    Coord c;
	    projectCoord(&OC, depthC, &c);
	    drawTriangle(&opcn, &c, &a,
			 (depthC - depthB) * kC + depthB,
			 depthC, depthA,
			 triangle,
			 &VW, W, U,
			 &nBC, normalC, normalA);
	} else if (depthC < getNearplan()) {
	    Point CA, CB;
	    diffPoint(A, C, &CA);
	    diffPoint(B, C, &CB);

	    float kA = (getNearplan() - depthC) / 
		scalarProduct(&getCamera()->j, &CA);
	    float kB = (getNearplan() - depthC) / 
		scalarProduct(&getCamera()->j, &CB);

	    Point OpA, OpB;
	    setPoint(&OpA,
		     C->x + kA * CA.x - getCamera()->O.x,
		     C->y + kA * CA.y - getCamera()->O.y,
		     C->z + kA * CA.z - getCamera()->O.z);
	    setPoint(&OpB,
		     C->x + kB * CB.x - getCamera()->O.x,
		     C->y + kB * CB.y - getCamera()->O.y,
		     C->z + kB * CB.z - getCamera()->O.z);

	    Coord opan, b, opbn;
	    projectCoord(&OpA, getNearplan(), &opan);
	    projectCoord(&OB, depthB, &b);
	    projectCoord(&OpB, getNearplan(), &opbn);

	    Texture WU, WV;
	    setTexture(&WU,
		       (U->x - W->x) * kA + W->x,
		       (U->y - W->y) * kA + W->y);
	    setTexture(&WV,
		       (V->x - W->x) * kB + W->x,
		       (V->y - W->y) * kB + W->y);

	    Point nCA, nCB;
	    setPoint(&nCA,
		     (normalA->x - normalC->x) * kA + normalC->x,
		     (normalA->y - normalC->y) * kA + normalC->y,
		     (normalA->z - normalC->z) * kA + normalC->z);
	    setPoint(&nCB,
		     (normalB->x - normalC->x) * kB + normalC->x,
		     (normalB->y - normalC->y) * kB + normalC->y,
		     (normalB->z - normalC->z) * kB + normalC->z);
	    drawTriangle(&opan, &b, &opan,
			 (depthA - depthC) * kA + depthC,
			 depthB,
			 (depthB - depthC) * kB + depthC,
			 triangle,
			 &WU, V, &WV,
			 &nCA, normalB, &nCB);

	    Coord a;
	    projectCoord(&OA, depthA, &a);
	    drawTriangle(&opan, &a, &b,
			 (depthA - depthC) * kA + depthC,
			 depthA, depthB,
			 triangle,
			 &WU, U, V,
			 &nCA, normalA, normalB);
	}
    }
}
