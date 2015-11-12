#ifndef BUILD_H
#define BUILD_H

#include "point.h"
#include "position.h"
#include "texture.h"

typedef struct Vertex {
    int point;
    int normal;
    int coord;
} Vertex;

typedef struct Face {
    Vertex vertices[3];
} Face;

typedef struct Solid {
    int numVertices;
    int numNormals;
    int numCoords;
    int numFaces;
    Point origin;
    Point *vertices;
    Point *normals;
    Position *coords;
    Texture *texture;
    Face *faces;
} Solid;

#endif //BUILD_H
