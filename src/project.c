#include <math.h>

#include "project.h"
#include "point.h"
#include "coord.h"
#include "draw.h"
#include "color.h"
#include "scene.h"
#include "texture.h"
#include "pixel.h"

// return the vector between camera.O and the intersection of (AB) 
// and the NEARPLAN
static void projectPoint(Lens *l, const Point *A, const Point *B, Point *S)
{
    Frame *p = getPosition(l);
    Point AB, d;
    diffPoint(B, A, &AB);
    diffPoint(A, &p->O, &d);
    float k = (getNearplan(l) - scalarProduct(&p->j, &d)) / 
	scalarProduct(&p->j, &AB);

    setPoint(S,
	     A->x + k * AB.x - p->O.x,
	     A->y + k * AB.y - p->O.y,
	     A->z + k * AB.z - p->O.z);
}

// return the Coord projection of a vector between scene.camera.O
// and A, knowing the depth of A
static void projectCoord(Lens *l, const Point *OA, float depth, Coord *S)
{    
    int sW = getScreenWidth(l);
    int sH = getScreenHeight(l);
    Frame *p = getPosition(l);

    S->w = sW / 
	(2. * tan(getHfov(l) / 2.)) * 
	scalarProduct(&p->i, OA) / 
	depth + sW / 2;
    S->h = -sH / 
	(2. * tan(getWfov(l) / 2.)) * 
	scalarProduct(&p->k, OA) / 
	depth + sH / 2;
}

void projectVertex(Lens *l, const Point *A, const Color *color)
{
    Point OA;
    Frame *p = getPosition(l);

    diffPoint(A, &p->O, &OA);
    float depthA = scalarProduct(&p->j, &OA);

    Coord t;
    projectCoord(l, &OA, depthA, &t);
    if (depthA > getNearplan(l))
	drawPixel(l, &t, depthA, color);
}

void projectSegment(Lens *l, const Point *A, const Point *B, const Color *color)
{
    Point OA, OB, AB;
    Frame *camera = getPosition(l);
    float nearplan = getNearplan(l);
    
    diffPoint(A, &camera->O, &OA);
    diffPoint(B, &camera->O, &OB);
    float depthA = scalarProduct(&camera->j, &OA);
    float depthB = scalarProduct(&camera->j, &OB);

    Coord t, u;
    if (depthA > nearplan && depthB > nearplan) {
	projectCoord(l, &OA, depthA, &t);
	projectCoord(l, &OB, depthB, &u);
	drawSegment(l, &t, &u, depthA, depthB, color);
    } else if (depthA < nearplan && depthB > nearplan) {
	projectPoint(l, A, B, &AB);
	projectCoord(l, &AB, nearplan, &t);
	projectCoord(l, &OB, depthB, &u);
    } else if (depthA > nearplan && depthB < nearplan) {
	projectPoint(l, A, B, &AB);
	projectCoord(l, &OA, depthA, &t);
	projectCoord(l, &AB, nearplan, &u);
    } else
	return;

    drawSegment(l, &t, &u, depthA, depthB, color);
}

static void cutInOneTriangle(Lens *l,
			     const Point *A, const Point *B, const Point *C,
			     float depthA, float depthB, float depthC,
			     const Point *OA,
			     Texture *triangle,
			     const Position *U, 
			     const Position *V, 
			     const Position *W,
			     const Point *normalA, 
			     const Point *normalB,
			     const Point *normalC)
{
    float nearplan = getNearplan(l);
    Frame *camera = getPosition(l);

    Point AB, AC;
    diffPoint(B, A, &AB);
    diffPoint(C, A, &AC);
    
    float kB = (nearplan - depthA) / scalarProduct(&camera->j, &AB);
    float kC = (nearplan - depthA) / scalarProduct(&camera->j, &AC);
    
    Point OpB, OpC;
    setPoint(&OpB, 
	     A->x + kB * AB.x - camera->O.x,
	     A->y + kB * AB.y - camera->O.y,
	     A->z + kB * AB.z - camera->O.z);
    setPoint(&OpC,
	     A->x + kC * AC.x - camera->O.x,
	     A->y + kC * AC.y - camera->O.y,
	     A->z + kC * AC.z - camera->O.z);
    
    Coord a, bn, cn;
    projectCoord(l, OA, depthA, &a);
    projectCoord(l, &OpB, nearplan, &bn);
    projectCoord(l, &OpC, nearplan, &cn);
    
    Position UV, UW;
    setPosition(&UV,
	       (V->x - U->x) * kB + U->x,
	       (V->y - U->y) * kB + U->y);
    setPosition(&UW,
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

    Color lA, lB, lC;
    calculateLightScene(A, normalA, &lA); 
    calculateLightScene(&OpB, &nAB, &lB); 
    calculateLightScene(&OpC, &nAC, &lC); 

    Pixel pA, pB, pC;
    setPixel(&pA, &a, depthA, &lA, U);
    setPixel(&pB, &bn, (depthB - depthA) * kB + depthA, &lB, &UV);
    setPixel(&pC, &cn, (depthC - depthA) * kC + depthA, &lC, &UW);
  
    drawTriangle(l, triangle, &pA, &pB, &pC);
}

static void cutInTwoTriangles(Lens *l,
			      const Point *A, const Point *B, const Point *C,
			      float depthA, float depthB, float depthC,
			      const Point *OB, const Point *OC,
			      Texture *triangle,
			      const Position *U, 
			      const Position *V, 
			      const Position *W,
			      const Point *normalA, 
			      const Point *normalB,
			      const Point *normalC)
{
    float nearplan = getNearplan(l);
    Frame *camera = getPosition(l);

    Point AB, AC;
    diffPoint(B, A, &AB);
    diffPoint(C, A, &AC);
    
    float kB = (nearplan - depthA) / scalarProduct(&camera->j, &AB);
    float kC = (nearplan - depthA) / scalarProduct(&camera->j, &AC);
    
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
    projectCoord(l, OB, depthB, &b);
    projectCoord(l, OC, depthC, &c);
    projectCoord(l, &OpB, nearplan, &opbn);
    projectCoord(l, &OpC, nearplan, &opcn);
    
    Position VU, WU;
    setPosition(&VU, (V->x - U->x) * kB + U->x, (V->y - U->y) * kB + U->y);
    setPosition(&WU, (W->x - U->x) * kC + U->x, (W->y - U->y) * kC + U->y);
    
    Point nAB, nAC;
    setPoint(&nAB,
	     (normalB->x - normalA->x) * kB + normalA->x,
	     (normalB->y - normalA->y) * kB + normalA->y,
	     (normalB->z - normalA->z) * kB + normalA->z);
    setPoint(&nAC,
	     (normalC->x - normalA->x) * kC + normalA->x,
	     (normalC->y - normalA->y) * kC + normalA->y,
	     (normalC->z - normalA->z) * kC + normalA->z);

    Color lB, lC, lBn, lCn;
    calculateLightScene(B, normalB, &lB); 
    calculateLightScene(C, normalC, &lC); 
    calculateLightScene(&OpB, &nAB, &lBn); 
    calculateLightScene(&OpC, &nAC, &lCn); 

    Pixel pB, pC, pBn, pCn;
    setPixel(&pB, &b, depthB, &lB, V);
    setPixel(&pC, &c, depthC, &lC, W);
    setPixel(&pBn, &opbn, (depthB - depthA) * kB + depthA, &lBn, &VU);
    setPixel(&pCn, &opcn, (depthC - depthA) * kC + depthA, &lCn, &WU);
  
    drawTriangle(l, triangle, &pBn, &pC, &pCn);
    drawTriangle(l, triangle, &pBn, &pB, &pC);
}

void projectTriangle(Lens *l, const Point *A, const Point *B, const Point *C,
		     Texture *triangle,
		     const Position *U, const Position *V, const Position *W,
		     const Point *normalA, 
		     const Point *normalB,
		     const Point *normalC)
{
    Frame *camera = getPosition(l);
    float nearplan = getNearplan(l);
    Point OA, OB, OC;
    diffPoint(A, &camera->O, &OA);
    diffPoint(B, &camera->O, &OB);
    diffPoint(C, &camera->O, &OC);
    float depthA = scalarProduct(&camera->j, &OA);
    float depthB = scalarProduct(&camera->j, &OB);
    float depthC = scalarProduct(&camera->j, &OC);

    if (depthA > nearplan && depthB > nearplan && depthC > nearplan){
	Pixel pA, pB, pC;
	Coord a, b, c;
	Color lA, lB, lC;
	
	projectCoord(l, &OA, depthA, &a);
	projectCoord(l, &OB, depthB, &b);
	projectCoord(l, &OC, depthC, &c);

	calculateLightScene(A, normalA, &lA);
	calculateLightScene(B, normalB, &lB);
	calculateLightScene(C, normalC, &lC);

	setPixel(&pA, &a, depthA, &lA, U);
	setPixel(&pB, &b, depthB, &lB, V);
	setPixel(&pC, &c, depthC, &lC, W);

	drawTriangle(l, triangle, &pA, &pB, &pC);
    } else if (depthB > nearplan && depthC > nearplan){
	cutInTwoTriangles(l, A, B, C,
			  depthA, depthB, depthC,
			  &OB, &OC,
			  triangle,
			  U, V, W,
			  normalA, normalB, normalC);
    } else if (depthC > nearplan && depthA > nearplan){
	cutInTwoTriangles(l, B, C, A,
			  depthB, depthC, depthA,
			  &OC, &OA,
			  triangle,
			  V, W, U,
			  normalB, normalC, normalA);
    } else if (depthA > nearplan && depthB > nearplan){
	cutInTwoTriangles(l, C, A, B,
			  depthC, depthA, depthB,
			  &OA, &OB,
			  triangle,
			  W, U, V,
			  normalC, normalA, normalB);
    } else if (depthA > nearplan){
	cutInOneTriangle(l, A, B, C,
			  depthA, depthB, depthC,
			  &OA,
			  triangle,
			  U, V, W,
			  normalA, normalB, normalC);
    } else if (depthB > nearplan){
	cutInOneTriangle(l, B, C, A,
			 depthB, depthC, depthA,
			 &OB,
			 triangle,
			 V, W, U,
			 normalB, normalC, normalA);
    } else if (depthC > nearplan){
	cutInOneTriangle(l, C, A, B,
			 depthC, depthA, depthB,
			 &OC,
			 triangle,
			 W, U, V,
			 normalC, normalA, normalB);
    }
}
