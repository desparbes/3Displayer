#include <math.h>

#include "project.h"
#include "point.h"
#include "coord.h"
#include "draw.h"
#include "color.h"
#include "scene.h"

typedef struct {
    Point *A, *B, *C;
    Point *normalA, *normalB, *normalC;
    Texture *textureA, *textureB, textureC;
    SDL_Surface *texture;
} Triangle;

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
    } else
	return;

    drawSegment(&t, &u, depthA, depthB, color);
}

static void cutInOneTriangle(const Point *A, const Point *B, const Point *C,
			     float depthA, float depthB, float depthC,
			     const Point *OA,
			     SDL_Surface *triangle,
			     const Texture *U, 
			     const Texture *V, 
			     const Texture *W,
			     const Point *normalA, 
			     const Point *normalB,
			     const Point *normalC)
{
    float nearplan = getNearplan();
    Frame *camera = getCamera();
    Point AB, AC;
    Point OpB, OpC;

    diffPoint(B, A, &AB);
    diffPoint(C, A, &AC);
    
    float kB = (nearplan - depthA) / scalarProduct(&camera->j, &AB);
    float kC = (nearplan - depthA) / scalarProduct(&camera->j, &AC);
    
    setPoint(&OpB, 
	     A->x + kB * AB.x - camera->O.x,
	     A->y + kB * AB.y - camera->O.y,
	     A->z + kB * AB.z - camera->O.z);
    setPoint(&OpC,
	     A->x + kC * AC.x - camera->O.x,
	     A->y + kC * AC.y - camera->O.y,
	     A->z + kC * AC.z - camera->O.z);
    
    Coord a, bn, cn;
    projectCoord(OA, depthA, &a);
    projectCoord(&OpB, nearplan, &bn);
    projectCoord(&OpC, nearplan, &cn);
    
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
}

static void cutInTwoTriangles(const Point *A, const Point *B, const Point *C,
			      float depthA, float depthB, float depthC,
			      const Point *OB, const Point *OC,
			      SDL_Surface *triangle,
			      const Texture *U, 
			      const Texture *V, 
			      const Texture *W,
			      const Point *normalA, 
			      const Point *normalB,
			      const Point *normalC)
{
    float nearplan = getNearplan();
    Frame *camera = getCamera();
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
    projectCoord(OB, depthB, &b);
    projectCoord(OC, depthC, &c);
    projectCoord(&OpB, nearplan, &opbn);
    projectCoord(&OpC, nearplan, &opcn);
    
    Texture VU, WU;
    setTexture(&VU, (V->x - U->x) * kB + U->x, (V->y - U->y) * kB + U->y);
    setTexture(&WU, (W->x - U->x) * kC + U->x, (W->y - U->y) * kC + U->y);
    
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
}

void projectTriangle(const Point *A, const Point *B, const Point *C,
		     SDL_Surface *triangle,
		     const Texture *U, const Texture *V, const Texture *W,
		     const Point *normalA, 
		     const Point *normalB,
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

    if (depthA > nearplan && depthB > nearplan && depthC > nearplan){
	Coord a, b, c;
	projectCoord(&OA, depthA, &a);
	projectCoord(&OB, depthB, &b);
	projectCoord(&OC, depthC, &c);
	drawTriangle(&a, &b, &c,
		     depthA, depthB, depthC,
		     triangle,
		     U, V, W,
		     normalA, normalB, normalC);
    } else if (depthB > nearplan && depthC > nearplan){
	cutInTwoTriangles(A, B, C,
			  depthA, depthB, depthC,
			  &OB, &OC,
			  triangle,
			  U, V, W,
			  normalA, normalB, normalC);
    } else if (depthC > nearplan && depthA > nearplan){
	cutInTwoTriangles(B, C, A,
			  depthB, depthC, depthA,
			  &OC, &OA,
			  triangle,
			  V, W, U,
			  normalB, normalC, normalA);
    } else if (depthA > nearplan && depthB > nearplan){
	cutInTwoTriangles(C, A, B,
			  depthC, depthA, depthB,
			  &OA, &OB,
			  triangle,
			  W, U, V,
			  normalC, normalA, normalB);
    } else if (depthA > nearplan){
	cutInOneTriangle(A, B, C,
			  depthA, depthB, depthC,
			  &OA,
			  triangle,
			  U, V, W,
			  normalA, normalB, normalC);
    } else if (depthB > nearplan){
	cutInOneTriangle(B, C, A,
			  depthB, depthC, depthA,
			  &OB,
			  triangle,
			  V, W, U,
			  normalB, normalC, normalA);
    } else if (depthC > nearplan){
	cutInOneTriangle(C, A, B,
			  depthC, depthA, depthB,
			  &OC,
			  triangle,
			  W, U, V,
			  normalC, normalA, normalB);
    }
}
