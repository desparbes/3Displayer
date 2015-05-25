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
    Frame *camera = getCamera();
    diffPoint(B, A, &AB);
    diffPoint(A, &camera->O, &d);
    float k =
	(getNearplan() - scalarProduct(&camera->j, &d))
	/ scalarProduct(&camera->j, &AB);

    setPoint(S,
	     A->x + k * AB.x - camera->O.x,
	     A->y + k * AB.y - camera->O.y,
	     A->z + k * AB.z - camera->O.z);
}

// return the Coord projection of a vector between scene.camera.O
// and A, knowing the depth of A
void projectCoord(const Point *OA, float depth, Coord *S)
{
    Frame *camera = getCamera();
    int height = getScreenHeight(), width = getScreenWidth();
    
    S->w =  width / (2. * tan(getWfov() * M_PI / 360.)) * 
	scalarProduct(&camera->i, OA) / 
	depth + width / 2;
    S->h = -height / 
	(2. * tan(getHfov() * M_PI / 360.)) * 
	scalarProduct(&camera->k, OA) / 
	depth + height / 2;
}

void projectVertex(const Point *A, const Color *color)
{
    Point OA;
    Frame *camera = getCamera();
    diffPoint(A, &camera->O, &OA);
    float depthA = scalarProduct(&camera->j, &OA);

    Coord t;
    projectCoord(&OA, depthA, &t);
    if (depthA > getNearplan())
	drawPixel(&t, depthA, color);
}

void projectSegment(const Point *A, const Point *B, const Color *color)
{
    Point OA, OB, AB;
    Frame *camera = getCamera();
    float nearplan = getNearplan();
    
    diffPoint(A, &camera->O, &OA);
    diffPoint(B, &camera->O, &OB);
    float depthA = scalarProduct(&camera->j, &OA);
    float depthB = scalarProduct(&camera->j, &OB);

    Coord t, u;
    if (depthA > nearplan && depthB > nearplan) {
	projectCoord(&OA, depthA, &t);
	projectCoord(&OB, depthB, &u);
	drawSegment(&t, &u, depthA, depthB, color);
    } else if (depthA < nearplan && depthB > nearplan) {
	projectPoint(A, B, &AB);
	projectCoord(&AB, nearplan, &t);
	projectCoord(&OB, depthB, &u);
    } else if (depthA > nearplan && depthB < nearplan) {
	projectPoint(A, B, &AB);
	projectCoord(&OA, depthA, &t);
	projectCoord(&AB, nearplan, &u);
    }
    drawSegment(&t, &u, depthA, depthB, color);
}

void projectTriangle(const Point *A, const Point *B, const Point *C,
		     SDL_Surface *triangle,
		     const Texture *U, const Texture *V, const Texture *W,
		     const Point *normalA, const Point *normalB,
		     const Point *normalC)
{
    Frame *camera = getCamera();
    float nearplan = getNearplan();
    Point OA, OB, OC;
    diffPoint(A, &camera->O, &OA);
    diffPoint(B, &camera->O, &OB);
    diffPoint(C, &camera->O, &OC);
    float depthA = scalarProduct(&camera->j, &OA);
    float depthB = scalarProduct(&camera->j, &OB);
    float depthC = scalarProduct(&camera->j, &OC);
    int test = (depthA > nearplan) + 
	       (depthB > nearplan) + 
	       (depthC > nearplan);

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
	if (depthA > nearplan) {
	    Point b, c;
	    diffPoint(B, A, &b);
	    diffPoint(C, A, &c);

	    float kB = (nearplan - depthA) / 
		scalarProduct(&camera->j, &b);
	    float kC = (nearplan - depthA) / 
		scalarProduct(&camera->j, &c);

	    setPoint(&OB, 
		     A->x + kB * b.x - camera->O.x,
		     A->y + kB * b.y - camera->O.y,
		     A->z + kB * b.z - camera->O.z);
	    setPoint(&OC,
		     A->x + kC * c.x - camera->O.x,
		     A->y + kC * c.y - camera->O.y,
		     A->z + kC * c.z - camera->O.z);

	    Coord a, bn, cn;
	    projectCoord(&OA, depthA, &a);
	    projectCoord(&OB, nearplan, &bn);
	    projectCoord(&OC, nearplan, &cn);

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
		
	} else if (depthB > nearplan) {
	    Point c, a;
	    diffPoint(C, B, &c);
	    diffPoint(A, B, &a);

	    float kC = (nearplan - depthB) / 
		scalarProduct(&camera->j, &c);
	    float kA = (nearplan - depthB) / 
		scalarProduct(&camera->j, &a);

	    setPoint(&OC,
		     B->x + kC * c.x - camera->O.x,
		     B->y + kC * c.y - camera->O.y,
		     B->z + kC * c.z - camera->O.z);
	    setPoint(&OA,
		     B->x + kA * a.x - camera->O.x,
		     B->y + kA * a.y - camera->O.y,
		     B->z + kA * a.z - camera->O.z);

	    Coord b, cn, an;
	    projectCoord(&OB, depthB, &b);
	    projectCoord(&OC, nearplan, &cn);
	    projectCoord(&OA, nearplan, &an);

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

	} else if (depthC > nearplan) {
	    Point a,b;
	    diffPoint(A, C, &a);
	    diffPoint(B, C, &b);

	    float kA = (nearplan - depthC) / 
		scalarProduct(&camera->j, &a);
	    float kB = (nearplan - depthC) / 
		scalarProduct(&camera->j, &b);

	    setPoint(&OA,
		     C->x + kA * a.x - camera->O.x,
		     C->y + kA * a.y - camera->O.y,
		     C->z + kA * a.z - camera->O.z);
	    setPoint(&OB,
		     C->x + kB * b.x - camera->O.x,
		     C->y + kB * b.y - camera->O.y,
		     C->z + kB * b.z - camera->O.z);

	    Coord c, an, bn;
	    projectCoord(&OC, depthC, &c);
	    projectCoord(&OA, nearplan, &an);
	    projectCoord(&OB, nearplan, &bn);

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
	if (depthA < nearplan) {
	    Point AB, AC;
	    diffPoint(B, A, &AB);
	    diffPoint(C, A, &AC);

	    float kB = (nearplan - depthA) / 
		scalarProduct(&camera->j, &AB);
	    float kC = (nearplan - depthA) / 
		scalarProduct(&camera->j, &AC);

	    Point OpB, OpC;
	    setPoint(&OpB,
		     A->x + kB * AB.x - camera->O.x,
		     A->y + kB * AB.y - camera->O.y,
		     A->z + kB * AB.z - camera->O.z);
	    setPoint(&OpC,
		     A->x + kC * AC.x - camera->O.x,
		     A->y + kC * AC.y - camera->O.y,
		     A->z + kC * AC.z - camera->O.z);

	    Coord b, c, opbn, opcn;
	    projectCoord(&OB, depthB, &b);
	    projectCoord(&OC, depthC, &c);
	    projectCoord(&OpB, nearplan, &opbn);
	    projectCoord(&OpC, nearplan, &opcn);

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

	    drawTriangle(&opbn, &b, &c,
			 (depthB - depthA) * kB + depthA,
			 depthB, depthC,
			 triangle,
			 &VU, V, W,
			 &nAB, normalB, normalC);
	} else if (depthB < nearplan) {
	    Point BC, BA;
	    diffPoint(C, B, &BC);
	    diffPoint(A, B, &BA);

	    float kC = (nearplan - depthB) / 
		scalarProduct(&camera->j, &BC);
	    float kA = (nearplan - depthB) / 
		scalarProduct(&camera->j, &BA);

	    Point OpC, OpA;
	    setPoint(&OpC,
		     B->x + kC * BC.x - camera->O.x,
		     B->y + kC * BC.y - camera->O.y,
		     B->z + kC * BC.z - camera->O.z);
	    setPoint(&OpA,
		     B->x + kA * BA.x - camera->O.x,
		     B->y + kA * BA.y - camera->O.y,
		     B->z + kA * BA.z - camera->O.z);

	    Coord c, a, opcn, opan;
	    projectCoord(&OC, depthC, &c);
	    projectCoord(&OA, depthA, &a);
	    projectCoord(&OpC, nearplan, &opcn);
	    projectCoord(&OpA, nearplan, &opan);

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

	    drawTriangle(&opcn, &c, &a,
			 (depthC - depthB) * kC + depthB,
			 depthC, depthA,
			 triangle,
			 &VW, W, U,
			 &nBC, normalC, normalA);
	} else if (depthC < nearplan) {
	    Point CA, CB;
	    diffPoint(A, C, &CA);
	    diffPoint(B, C, &CB);

	    float kA = (nearplan - depthC) / 
		scalarProduct(&camera->j, &CA);
	    float kB = (nearplan - depthC) / 
		scalarProduct(&camera->j, &CB);

	    Point OpA, OpB;
	    setPoint(&OpA,
		     C->x + kA * CA.x - camera->O.x,
		     C->y + kA * CA.y - camera->O.y,
		     C->z + kA * CA.z - camera->O.z);
	    setPoint(&OpB,
		     C->x + kB * CB.x - camera->O.x,
		     C->y + kB * CB.y - camera->O.y,
		     C->z + kB * CB.z - camera->O.z);

	    Coord a, b, opan, opbn;

	    projectCoord(&OA, depthA, &a);
	    projectCoord(&OB, depthB, &b);
	    projectCoord(&OpA, nearplan, &opan);	   
	    projectCoord(&OpB, nearplan, &opbn);

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

	    drawTriangle(&opan, &b, &opbn,
			 (depthA - depthC) * kA + depthC,
			 depthB,
			 (depthB - depthC) * kB + depthC,
			 triangle,
			 &WU, V, &WV,
			 &nCA, normalB, &nCB);

	    drawTriangle(&opan, &a, &b,
			 (depthA - depthC) * kA + depthC,
			 depthA, depthB,
			 triangle,
			 &WU, U, V,
			 &nCA, normalA, normalB);
	}
    }
}
