#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "build.h"
#include "point.h"
#include "position.h"
#include "texture.h"

#define MAXLENGTH 256

Solid *loadObject(const char *fileName, const char *bmpName)
{
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        perror(fileName);
        return NULL;
    }

    int a = 0, b = 0, c = 0, d = 0, n, stock, bufferSize = 1;
    Solid *solid = malloc(sizeof(Solid));
    char str[MAXLENGTH] = {0};
    char f;

    solid->numVertices = 0;
    solid->numNormals = 0;
    solid->numCoords = 0;
    solid->numSegments = 0;
    solid->numFaces = 0;

    while (fscanf(file, "%s", str) != EOF) {
        if (strcmp(str, "v") == 0)
            solid->numVertices++;
        else if (strcmp(str, "vn") == 0)
            solid->numNormals++;
        else if (strcmp(str, "vt") == 0)
            solid->numCoords++;
        else if (strcmp(str, "f") == 0) {
            n = 0;
            while (fscanf(file, "%c", &f) != EOF && f != '\n') {
                if (f == '/')
                    n++;
            }
            solid->numFaces += n / 2 - 2;
        }
    }
    solid->vertices = (Point*) malloc(solid->numVertices * sizeof(Point));
    solid->normals = (Point*) malloc(solid->numNormals * sizeof(Point));
    solid->coords = (Position*) malloc(solid->numCoords * sizeof(Position));
    solid->segments = (Segment*) malloc(bufferSize * sizeof(Segment));
    solid->faces = (Face *) malloc(solid->numFaces * sizeof(Face));

    if((solid->texture = loadTexture(bmpName)))
        printf("Texture successfully loaded\n");
    else
        printf("Error loading texture\n");

    rewind(file);
    while (fscanf(file, "%s", str) != EOF) {
        if (strcmp(str, "v") == 0) {
            if (fscanf
                (file, "%f %f %f\n",
                 &solid->vertices[a].y,
                 &solid->vertices[a].z,
                 &solid->vertices[a].x) != 3) {
                fprintf(stderr, "Error loading vertices\n");
                free(solid);
                fclose(file);
                return NULL;
            }
            a++;
        } else if (strcmp(str, "vn") == 0) {
            if (fscanf
                (file, "%f %f %f\n",
                 &solid->normals[b].y,
                 &solid->normals[b].z,
                 &solid->normals[b].x) != 3) {
                fprintf(stderr, "Error loading normals\n");
                free(solid);
                fclose(file);
                return NULL;
            }
            b++;
        } else if (strcmp(str, "vt") == 0) {
            if (fscanf
                (file, "%f %f\n",
                 &solid->coords[c].x,
                 &solid->coords[c].y) != 2) {
                fprintf(stderr, "Error loading texture coordinates\n");
                free(solid);
                fclose(file);
                return NULL;
            }
            solid->coords[c].y = 1 - solid->coords[c].y;
            c++;
        } else if (strcmp(str, "f") == 0) {

            stock = d;

            if (fscanf(file, "%d/%d/%d",
                       &solid->faces[d].vertices[0].point,
                       &solid->faces[d].vertices[0].coord,
                       &solid->faces[d].vertices[0].normal) != 3 ||
                fscanf(file, "%d/%d/%d",
                       &solid->faces[d].vertices[1].point,
                       &solid->faces[d].vertices[1].coord,
                       &solid->faces[d].vertices[1].normal) != 3 ||
                fscanf(file, "%d/%d/%d",
                       &solid->faces[d].vertices[2].point,
                       &solid->faces[d].vertices[2].coord,
                       &solid->faces[d].vertices[2].normal) != 3) {
                fprintf(stderr, "Error during faces enumeration\n");
                free(solid);
                fclose(file);
                return NULL;
            }

            solid->faces[d].vertices[0].point--;
            solid->faces[d].vertices[0].coord--;
            solid->faces[d].vertices[0].normal--;
            solid->faces[d].vertices[1].point--;
            solid->faces[d].vertices[1].coord--;
            solid->faces[d].vertices[1].normal--;
            solid->faces[d].vertices[2].point--;
            solid->faces[d].vertices[2].coord--;
            solid->faces[d].vertices[2].normal--;

            Segment u;
            setSegmentBuild(&u,
                            solid->faces[d].vertices[0].point,
                            solid->faces[d].vertices[1].point);
            addSegmentBuild(solid, &u, &bufferSize);
            setSegmentBuild(&u,
                            solid->faces[d].vertices[1].point,
                            solid->faces[d].vertices[2].point);
            addSegmentBuild(solid, &u, &bufferSize);
            setSegmentBuild(&u,
                            solid->faces[d].vertices[2].point,
                            solid->faces[d].vertices[0].point);
            addSegmentBuild(solid, &u, &bufferSize);


            d++;

            while (fscanf(file, "%d/%d/%d",
                          &solid->faces[d].vertices[2].point,
                          &solid->faces[d].vertices[2].coord,
                          &solid->faces[d].vertices[2].normal) == 3) {
                solid->faces[d].vertices[0].point =
                    solid->faces[stock].vertices[0].point;
                solid->faces[d].vertices[0].coord =
                    solid->faces[stock].vertices[0].coord;
                solid->faces[d].vertices[0].normal =
                    solid->faces[stock].vertices[0].normal;

                solid->faces[d].vertices[1].point =
                    solid->faces[d - 1].vertices[2].point;
                solid->faces[d].vertices[1].coord =
                    solid->faces[d - 1].vertices[2].coord;
                solid->faces[d].vertices[1].normal =
                    solid->faces[d - 1].vertices[2].normal;

                solid->faces[d].vertices[2].point--;
                solid->faces[d].vertices[2].coord--;
                solid->faces[d].vertices[2].normal--;

                setSegmentBuild(&u,
                                solid->faces[d].vertices[1].point,
                                solid->faces[d].vertices[2].point);
                addSegmentBuild(solid, &u, &bufferSize);

                setSegmentBuild(&u,
                                solid->faces[d].vertices[2].point,
                                solid->faces[d].vertices[0].point);
                addSegmentBuild(solid, &u, &bufferSize);

                d++;
            }
        }
    }
    qsort(solid->segments, solid->numSegments, sizeof(Segment),
          compareSegmentBuild);
    formatSegmentBuild(solid);
    printf("Object successfully loaded\n");
    fclose(file);
    return solid;
}
