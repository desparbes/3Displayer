#ifndef DATA_H
#define DATA_H

#include <SDL/SDL.h>

#define BIT        32
#define WFOV       80
#define HFOV       60
#define WIDTH      1200
#define HEIGHT     900
#define MAXLENGTH  256
#define SPEED      0.1
#define ANGLE      0.01
#define NEARPLAN   1
#define EPSILON    0.001

typedef struct Point {
    float x;
    float y;
    float z;
} Point;

typedef struct Texture {
    float x;
    float y;
} Texture;

typedef struct Coord {
    int w;
    int h;
} Coord;

typedef struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

typedef struct Camera {
    Point position;
    Point i;
    Point j;
    Point k;
} Camera;

typedef struct Vertex {
    int point;
    int normal;
    int coord;
} Vertex;

typedef struct Face {
    Vertex vertices[3];
    int texture;
} Face;

typedef struct Solid {
    int numVertices;
    int numNormals;
    int numCoords;
    int numTextures;
    int numFaces;
    Point *vertices;
    Point *normals;
    Texture *coords;
    SDL_Surface **textures;
    Face *faces;
} Solid;

extern float wCoef;
extern float hCoef;
extern Color red;
extern Color green;
extern Color blue;
extern Point light;
extern SDL_Surface *screen;
extern float *zBuffer;
extern Camera user;
extern Point PointO;
extern Point I;
extern Point J;
extern Point K;

void setPoint(Point *p, float x, float y, float z);
void setCoord(Coord *c, int w, int h);
void setTexture(Texture *t, float x, float y);

#endif	//DATA_H
