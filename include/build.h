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

// A < B
typedef struct Segment {
    int A;
    int B;
} Segment;

typedef struct Face {
    Vertex vertices[3];
} Face;

typedef struct Solid {
    int numVertices;
    int numNormals;
    int numCoords;
    int numSegments;
    int numFaces;
    Point origin;
    Point *vertices;
    Point *normals;
    Position *coords;
    Texture *texture;
    Segment *segments;
    Face *faces;
} Solid;

void setSegmentBuild(Segment *u, int A, int B);
int compareSegmentBuild(const void *u, const void *v);
void formatSegmentBuild(Solid *solid);
void addSegmentBuild(Solid *solid, const Segment *segment, int *bufferSize);

#endif //BUILD_H
