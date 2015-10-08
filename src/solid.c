#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <math.h>
#include <string.h>

#include "point.h"
#include "color.h"
#include "texture.h"
#include "project.h"
#include "solid.h"
#include "equation.h"
#include "lens.h"

#define MAXLENGTH 256
#define EPSILON 0.001

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
    Texture *coords;
    SDL_Surface *texture;
    Face *faces;
} Solid;

Point *getOriginSolid(Solid *solid)
{
    return &solid->origin;
}

static void calculateOriginSolid(Solid *solid)
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
  
void wireframeSolid(Lens *l, const Solid *solid, const Color *color)
{
    for (int i = 0; i < solid->numFaces; i++) {
	Face *f = &solid->faces[i];
	for (int k = 0; k < 3; k++) {
	    int point1 = f->vertices[k].point;
	    int point2 = f->vertices[(k+1)%3].point;
	    projectSegment(l, &solid->vertices[point1],
			   &solid->vertices[point2], color);
	}
    }
}

void vertexSolid(Lens *l, const Solid *solid, const Color *color)
{
    for (int i = 0; i < solid->numVertices; ++i)
	projectVertex(l, &solid->vertices[i], color);
}

void normalSolid(Lens *l, const Solid *solid, const Color *color)
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

// load .obj files
Solid *loadSolid(const char *fileName, const char *bmpName)
{
    FILE *file = fopen(fileName, "r");

    if (file == NULL) {
	perror(fileName);
        return NULL;
    }

    int a = 0, b = 0, c = 0, d = 0, n, stock;
    Solid *solid = malloc(sizeof(Solid));
    char str[MAXLENGTH] = { 0 };
    char f;
    
    solid->numVertices = 0;
    solid->numNormals = 0;
    solid->numCoords = 0;
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
    solid->coords = (Texture*) malloc(solid->numCoords * sizeof(Texture));
    solid->faces = (Face *) malloc(solid->numFaces * sizeof(Face));
 
   if ((solid->texture = SDL_LoadBMP(bmpName)) == NULL)
       fprintf(stderr, "%s\n", SDL_GetError());
   else
       printf("Texture successfully loaded\n");

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

	    if (fscanf
		(file, "%d/%d/%d", &solid->faces[d].vertices[0].point,
		 &solid->faces[d].vertices[0].coord,
		 &solid->faces[d].vertices[0].normal) != 3
		|| fscanf(file, "%d/%d/%d",
			  &solid->faces[d].vertices[1].point,
			  &solid->faces[d].vertices[1].coord,
			  &solid->faces[d].vertices[1].normal) != 3
		|| fscanf(file, "%d/%d/%d",
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

	    d++;

	    while (fscanf
		   (file, "%d/%d/%d", &solid->faces[d].vertices[2].point,
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

		d++;
	    }
	}
    }
    calculateOriginSolid(solid);
    printf("Solid successfully loaded\n");
    fclose(file);
    return solid;
}

Solid *equationSolid(const char *eqName, const char *bmpName)
{
    float minS, maxS, minT, maxT;
    int precisionS, precisionT;
    char x[MAXLENGTH];
    char y[MAXLENGTH];
    char z[MAXLENGTH];
    
    if (!initEquation(&minS, &maxS, &precisionS,
		      &minT, &maxT, &precisionT,
		      x, y, z, MAXLENGTH, eqName)) {
	fprintf(stderr, "Error loading equation\n");
	return NULL;
    } else 
	printf("Equation successfully initialized\n");

    Solid *solid = malloc(sizeof(Solid));
    int p, f = 0;
    float s = minS, t = minT;
    float ds = (maxS - minS) / (precisionS - 1);
    float dt = (maxT - minT) / (precisionT - 1);

    solid->numVertices = precisionS * precisionT;
    solid->numNormals = solid->numVertices;
    solid->numCoords = 4;
    solid->numFaces = 2 * (precisionS - 1) * (precisionT - 1);

    solid->vertices = malloc(solid->numVertices * sizeof(Point));
    solid->normals = malloc(solid->numNormals * sizeof(Point));
    solid->coords = malloc(solid->numCoords * sizeof(Texture));
    solid->faces = malloc(solid->numFaces * sizeof(Face));

    setTexture(&solid->coords[0], 0., 0.);
    setTexture(&solid->coords[1], 0., 1.);
    setTexture(&solid->coords[2], 1., 0.);
    setTexture(&solid->coords[3], 1., 1.);

    if ((solid->texture = SDL_LoadBMP(bmpName)) == NULL)
	fprintf(stderr, "%s\n", SDL_GetError());
    
    if (solid->numVertices > 0)
	getValueFromEquation(x, y, z, s, t, &solid->vertices[0]);	

    for (p = 0; p < solid->numVertices; p++) {
	Point *A;
	Point *B;
	Point *O = &solid->vertices[p];
	Point *normal = &solid->normals[p];
	Point u;
	Point v;

        if (p == solid->numVertices - 1) { // north-east
	    A = &solid->vertices[p - 1];
	    B = &solid->vertices[p - precisionS];
	} else if (p % precisionS == precisionS - 1) { // east
	    A = &solid->vertices[p + precisionS];
	    B = &solid->vertices[p - 1];

	    getValueFromEquation(x, y, z, s, t + dt, A);
	    s = minS;
	    t += dt;
	} else if (p >= (solid->numVertices - precisionS)) { // north
	    A = &solid->vertices[p - precisionS];
	    B = &solid->vertices[p + 1];

	    s += ds;
	} else if (p < precisionS) { // south
	    A = &solid->vertices[p + 1];
	    B = &solid->vertices[p + precisionS];
	    
	    getValueFromEquation(x, y, z, s + ds, t, A);
	    getValueFromEquation(x, y, z, s, t + dt, B);
	    s += ds;
	} else { // elsewhere
	    A = &solid->vertices[p + 1];
	    B = &solid->vertices[p + precisionS];
	    
	    getValueFromEquation(x, y, z, s, t + dt, B);
	    s += ds;
	}
	diffPoint(A, O, &u);
	diffPoint(B, O, &v);
	pointProduct(&u, &v, normal);
	normalizePoint(normal, normal);
    }

    p = 0;
    while (f < solid->numFaces) {
	if (p % precisionS != precisionS - 1) {
	    solid->faces[f].vertices[0].point = p;
	    solid->faces[f].vertices[1].point = p + 1;
	    solid->faces[f].vertices[2].point = p + precisionS;
	    solid->faces[f].vertices[0].normal = p;
	    solid->faces[f].vertices[1].normal = p + 1;
	    solid->faces[f].vertices[2].normal = p + precisionS;
	    solid->faces[f].vertices[0].coord = 1;
	    solid->faces[f].vertices[1].coord = 3;
	    solid->faces[f].vertices[2].coord = 0;

	    f++;

	    solid->faces[f].vertices[0].point = p + precisionS;
	    solid->faces[f].vertices[1].point = p + 1;
	    solid->faces[f].vertices[2].point = p + 1 + precisionS;
	    solid->faces[f].vertices[0].normal = p + precisionS;
	    solid->faces[f].vertices[1].normal = p + 1;
	    solid->faces[f].vertices[2].normal = p + 1 + precisionS;
	    solid->faces[f].vertices[0].coord = 0;
	    solid->faces[f].vertices[1].coord = 3;
	    solid->faces[f].vertices[2].coord = 2;

	    f++;
	}
	p++;
    }
    calculateOriginSolid(solid);
    freeEquation();
    printf("Equation successfully loaded\n");
    return solid;
}

void freeSolid(Solid *solid)
{
    if (solid->texture != NULL)
	SDL_FreeSurface(solid->texture);
    free(solid->vertices);
    free(solid->normals);
    free(solid->coords);
    free(solid->faces);
    free(solid);
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
