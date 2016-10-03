#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "point.h"
#include "color.h"
#include "project.h"
#include "equation.h"
#include "object.h"
#include "lens.h"
#include "frame.h"
#include "texture.h"
#include "build.h"

#define MAXLENGTH 256
#define EPSILON 0.001
#define min(x, y) ((x) < (y) ? (x) : (y))

static const char *getExtension(const char *file)
{
    if (file == NULL)
        return "";
    const char *e = strrchr(file, '.');
    if (e == NULL)
        e = file;
    return e;
}

Solid *loadSolid(const char *fileName, const char *bmpName)
{
    const char *ext = getExtension(fileName);
    if (strcmp(ext, ".obj") == 0)
        return loadObject(fileName, bmpName);
    else if (strcmp(ext, ".eq") == 0)
        return loadEquation(fileName, bmpName);
    fprintf(stderr, "Extension non reconnue: '%s'\n", ext);
    return NULL;
}

void calculateOriginSolid(Solid *solid)
{
    for (int i = 0; i < solid->numVertices; i++) {
        solid->origin.x += solid->vertices[i].x;
        solid->origin.y += solid->vertices[i].y;
        solid->origin.z += solid->vertices[i].z;
    }
    solid->origin.x /= solid->numVertices;
    solid->origin.y /= solid->numVertices;
    solid->origin.z /= solid->numVertices;
}

Point *getOriginSolid(Solid *solid)
{
    return &solid->origin;
}

void translateSolid(Solid *solid, float x, float y, float z)
{
    for (int i = 0; i < solid->numVertices; i++)
        translatePoint(&solid->vertices[i], x, y, z);
    translatePoint(&solid->origin, x, y, z);
}

void scaleSolid(Solid *solid, const Point *O, float scale)
{
    for (int i = 0; i < solid->numVertices; i++)
        scalePoint(&solid->vertices[i], O, scale);
    scalePoint(&solid->origin, O, scale);
}

void rotSolidXAxis(Solid *solid, const Point *O, float phi)
{
    for (int i = 0; i < solid->numVertices; i++)
        rotPointXAxis(&solid->vertices[i], O, phi);
    rotPointXAxis(&solid->origin, O, phi);
}

void rotSolidYAxis(Solid *solid, const Point *O, float rho)
{
    for (int i = 0; i < solid->numVertices; i++)
        rotPointYAxis(&solid->vertices[i], O, rho);
    rotPointXAxis(&solid->origin, O, rho);
}

void rotSolidZAxis(Solid *solid, const Point *O, float theta)
{
    for (int i = 0; i < solid->numVertices; i++)
        rotPointZAxis(&solid->vertices[i], O, theta);
    rotPointXAxis(&solid->origin, O, theta);
}

void vertexSolid(Lens *l, const Solid *solid, Color color)
{
    for (int i = 0; i < solid->numVertices; ++i)
        projectVertex(l, &solid->vertices[i], color);
}

void wireframeSolid(Lens *l, const Solid *solid, Color color)
{
    for (int i = 0; i < solid->numSegments; i++) {
        projectSegment(l,
                       &solid->vertices[solid->segments[i].A],
                       &solid->vertices[solid->segments[i].B],
                       color);
    }
}

void normalSolid(Lens *l, const Solid *solid, Color color)
{
    for (int i = 0; i < solid->numFaces; i++) {
        Face *f = &solid->faces[i];
        for (int k = 0; k < 3; k++) {
            Point tmp;
            int point = f->vertices[k].point;
            Point normal = solid->normals[f->vertices[k].normal];

            setPoint(&tmp, 0., 0., 0.);
            scalePoint(&normal, &tmp, 0.1);
            sumPoint(&solid->vertices[point], &normal, &tmp);
            projectSegment(l, &solid->vertices[point], &tmp, color);
        }
    }
}

void drawSolid(Lens *l, const Solid * solid)
{
    for (int i = 0; i < solid->numFaces; i++)
        projectTriangle(l,
                        &solid->vertices[solid->faces[i].vertices[0].point],
                        &solid->vertices[solid->faces[i].vertices[1].point],
                        &solid->vertices[solid->faces[i].vertices[2].point],
                        solid->texture,
                        &solid->coords[solid->faces[i].vertices[0].coord],
                        &solid->coords[solid->faces[i].vertices[1].coord],
                        &solid->coords[solid->faces[i].vertices[2].coord],
                        &solid->normals[solid->faces[i].vertices[0].normal],
                        &solid->normals[solid->faces[i].vertices[1].normal],
                        &solid->normals[solid->faces[i].vertices[2].normal]);
}

void drawFrame(Lens *l, Frame *frame)
{
    Color red=COLOR(255,0,0), blue=COLOR(0,255,0), green=COLOR(0,0,255);

    projectSegment(l, &frame->O, &frame->i, red);
    projectSegment(l, &frame->O, &frame->j, blue);
    projectSegment(l, &frame->O, &frame->k, green);
}

void freeSolid(Solid *solid)
{
    freeTexture(solid->texture);
    free(solid->vertices);
    free(solid->normals);
    free(solid->coords);
    free(solid->segments);
    free(solid->faces);
    free(solid);
}
