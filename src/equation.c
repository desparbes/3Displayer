#include <stdio.h>
#include <stdlib.h>

#include "build.h"
#include "position.h"
#include "texture.h"
#include "point.h"
#include "parametric.h"
#include "hypergrid.h"

typedef struct Grid {
    int grid[3];
} Grid;

static int minimum(int a, int b)
{
    return a < b ? a : b;
}

static void setGrid(int A[3], int x, int y, int z, int result[3])
{
    result[0] = A[0] + x;
    result[1] = A[1] + y;
    result[2] = A[2] + z;
}

static int isInRange(int M[3], int lengths[3])
{
    for (int i = 0; i < 3; i++) {
        if (M[i] < 0 || M[i] >= lengths[i])
            return 0;
    }
    return 1;
}

void setNormal(Solid *solid, int O, int A, int B, int **normalBuffer,
               int dim, int *n, int side)
{
    Point u, v, normal;

    diffPoint(&solid->vertices[side ? B : A], &solid->vertices[O], &u);
    diffPoint(&solid->vertices[side ? A : B], &solid->vertices[O], &v);
    pointProduct(&u, &v, &normal);
    normalizePoint(&normal, &normal);

    normalBuffer[O][dim] = *n;
    solid->normals[*n] = normal;
    (*n)++;
}

static void setPointFace(Solid *solid, int f, int a, int b, int c)
{
    solid->faces[f].vertices[0].point = a;
    solid->faces[f].vertices[1].point = b;
    solid->faces[f].vertices[2].point = c;
}

static void setPositionFace(Solid *solid, int f, int a, int b, int c)
{
    solid->faces[f].vertices[0].coord = a;
    solid->faces[f].vertices[1].coord = b;
    solid->faces[f].vertices[2].coord = c;
}

static void setNormalFace(Solid *solid, int f, int a, int b, int c)
{
    solid->faces[f].vertices[0].normal = a;
    solid->faces[f].vertices[1].normal = b;
    solid->faces[f].vertices[2].normal = c;
}

static void setTriangle(Solid *solid, int **normalBuffer, int dim, int f,
                        int a, int b, int c, int A, int B, int C)
{
    setPointFace(solid, f, a, b, c);
    setPositionFace(solid, f, A, B, C);
    setNormalFace(solid, f,
                  normalBuffer[a][dim],
                  normalBuffer[b][dim],
                  normalBuffer[c][dim]);
}

static void setSquare(Solid *solid, int **normalBuffer,
                      int a, int b, int c, int d, int dim, int *f, int side)
{
    if (side) {
        setTriangle(solid, normalBuffer, dim, *f, a, b, c, 1, 3, 2);
        (*f)++;
        setTriangle(solid, normalBuffer, dim, *f, a, c, d, 1, 2, 0);
        (*f)++;
    } else {
        setTriangle(solid, normalBuffer, dim, *f, a, c, b, 1, 2, 3);
        (*f)++;
        setTriangle(solid, normalBuffer, dim, *f, a, d, c, 1, 0, 2);
        (*f)++;
    }
 }

static void updateInput(float *input, float *min, float *interval,
                        int *grid, int dim)
{
    for (int i = 0; i < dim; i++)
        input[i] = min[i] + interval[i] * grid[i];
}

static void mapOutput(float *output, Point *A, int dim)
{
    A->x = dim >= 1 ? output[0] : 0;
    A->y = dim >= 2 ? output[1] : 0;
    A->z = dim >= 3 ? output[2] : 0;
}

Solid *loadEquation(const char *eqName, const char *bmpName)
{
    float *min = NULL, *max = NULL;
    int *precision = NULL;
    int nbInput = 0;
    int nbOutput = 0;

    if (!initEquation(&min, &max, &precision, &nbInput, &nbOutput, eqName)) {
        fprintf(stderr, "Error loading equation\n");
        return NULL;
    }

    Solid *solid = malloc(sizeof(Solid));
    float *input = malloc(nbInput * sizeof(float));
    float *output = malloc(nbOutput * sizeof(float));
    float *interval = malloc(nbInput * sizeof(float));
    int dim = minimum(nbInput, 3);

    int lengths[3] = {1, 1, 1};
    for (int i = 0; i < dim; i++)
        lengths[i] = precision[i];

    int a, e = 0, n = 0, f = 0;
    for (int i = 0; i < nbInput; i++) {
        input[i] = min[i];
        interval[i] = (max[i] - min[i]) / (precision[i] - 1);
    }

    solid->numVertices = getNumVertices(precision, dim);
    solid->numSegments = getNumSegments(precision, dim);
    solid->numCoords = 4;
    solid->numNormals = getNumNormals(precision, dim);
    solid->numFaces = 2 * getNumFaces(precision, dim);

    if ((solid->texture = loadTexture(bmpName)))
        printf("Texture successfully loaded\n");
    else
        printf("Error loading texture\n");

    solid->vertices = malloc(solid->numVertices * sizeof(Point));
    solid->normals = malloc(solid->numNormals * sizeof(Point));
    solid->segments = malloc(solid->numSegments * sizeof(Segment));
    solid->faces = malloc(solid->numFaces * sizeof(Face));
    solid->coords = malloc(solid->numCoords * sizeof(Position));

    Grid *gridBuffer = malloc(solid->numVertices * sizeof(Grid));
    int **normalBuffer = malloc(solid->numVertices * sizeof(int *));
    for (int i = 0; i < solid->numVertices; i++)
        normalBuffer[i] = malloc(3 * sizeof(int));

    setPosition(&solid->coords[0], 0., 0.);
    setPosition(&solid->coords[1], 0., 1.);
    setPosition(&solid->coords[2], 1., 0.);
    setPosition(&solid->coords[3], 1., 1.);

    for (a = 0; a < solid->numVertices; a++) {
        getGridFromId(a, solid->numVertices, lengths,
                      gridBuffer[a].grid, 3);
        updateInput(input, min, interval, gridBuffer[a].grid, dim);
        getValueFromEquation(input, output);
        mapOutput(output, &solid->vertices[a], nbOutput);
    }

    for (int i = 0; i < solid->numVertices; i++) {
        int *O = gridBuffer[i].grid;
        for (int j = 0; j < 3; j++) {
            int side = O[j] == 0;
            if (side || O[j] == lengths[j] - 1) {
                int A[3];
                int B[3];
                int C[3];
                int D[3];

                setGrid(O, j == 2, j == 0, j == 1, A);
                setGrid(O, j == 1, j == 2, j == 0, B);
                int a = isInRange(A, lengths);
                int b = isInRange(B, lengths);

                if (a && b) {
                    setNormal(solid, i,
                              getIdFromGrid(lengths, A, 3),
                              getIdFromGrid(lengths, B, 3),
                              normalBuffer, j, &n, side);
                    continue;
                }
                setGrid(O, -(j == 2), -(j == 0), -(j == 1), C);
                int c = isInRange(C, lengths);

                if (b && c) {
                    setNormal(solid, i,
                              getIdFromGrid(lengths, B, 3),
                              getIdFromGrid(lengths, C, 3),
                              normalBuffer, j, &n, side);
                    continue;
                }
                setGrid(O, -(j == 1), -(j == 2), -(j == 0), D);
                int d = isInRange(D, lengths);

                if (c && d) {
                    setNormal(solid, i,
                              getIdFromGrid(lengths, C, 3),
                              getIdFromGrid(lengths, D, 3),
                              normalBuffer, j, &n, side);
                    continue;
                }

                if (d && a) {
                    setNormal(solid, i,
                              getIdFromGrid(lengths, D, 3),
                              getIdFromGrid(lengths, A, 3),
                              normalBuffer, j, &n, side);
                    continue;
                }
            }
        }
    }
    for (a = 0; a < solid->numVertices; a++) {
        int delta = 1;
        for (int i = 0; i < dim; i++) {
            if (gridBuffer[a].grid[i] < precision[i] - 1) {
                solid->segments[e].A = a;
                solid->segments[e].B = a + delta;
                e++;
            }
            delta *= precision[i];
        }
        int *A = gridBuffer[a].grid;
        for (int i = 0; i < 3; i++) {
            int C[3];
            setGrid(A, i != 0, i != 1, i != 2, C);
            int side = A[i] == 0;
            if (isInRange(C, lengths) &&
                (side || A[i] == lengths[i] - 1)) {
                int B[3];
                int D[3];
                setGrid(A, i == 1, i == 2, i == 0, B);
                setGrid(A, i == 2, i == 0, i == 1, D);
                int b = getIdFromGrid(precision, B, dim);
                int c = getIdFromGrid(precision, C, dim);
                int d = getIdFromGrid(precision, D, dim);
                setSquare(solid, normalBuffer, a, b, c, d, i, &f, side);
            }
        }
    }
    free(min);
    free(max);
    free(precision);
    free(interval);
    free(input);
    free(output);
    free(gridBuffer);
    freeEquation();
    for (int i = 0; i < solid->numVertices; i++)
        free(normalBuffer[i]);
    free(normalBuffer);
    printf("Equation successfully loaded\n");
    return solid;
}



