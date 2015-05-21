#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "data.h"
#include "basic.h"
#include "project.h"
#include "solid.h"

void wireframeSolid(const Solid *solid, const Color *color)
{
    int i, k;
    for (i = 0; i < solid->numFaces; i++) {
	Face *f = &solid->faces[i];
	for (k = 0; k < 3; k++) {
	    int point1 = f->vertices[k].point;
	    int point2 = f->vertices[(k+1)%2].point;
	    projectSegment(&solid->vertices[point1],
			   &solid->vertices[point2], color);
	}
    }
}

void vertexSolid(const Solid *solid, const Color *color)
{
    int i;
    for (i = 0; i < solid->numVertices; ++i)
	projectVertex(&solid->vertices[i], color);
}

void segmentSolid(const Solid *solid, const Color *color)
{
    int i, j;
    for (i = 0; i < solid->numVertices - 1; ++i) {
	for (j = i + 1; j < solid->numVertices; ++j)
	    projectSegment(&solid->vertices[i],
			   &solid->vertices[j], color);
    }
}

void normalSolid(const Solid *solid, const Color *color)
{
    int i, k;
    for (i = 0; i < solid->numFaces; i++) {
	Face *f = &solid->faces[i];
	for (k = 0; k < 3; k++) {
	    int point = f->vertices[k].point;
	    int normal = f->vertices[k].normal;
	    Point tmp;
	    sumPoint(&solid->vertices[point], &solid->normals[normal], &tmp);
	    projectSegment(&solid->vertices[point], &tmp, color);
	}
    }
}

// load .obj files
int loadSolid(Solid *solid, const char *fileName)
{
    FILE *file = fopen(fileName, "r");

    if (file == NULL) {
	perror(fileName);
	exit(EXIT_FAILURE);
    }

    int i = 0, a = 0, b = 0, c = 0, d = 0, n, stock;

    char str[MAXLENGTH] = { 0 };
    char f;

    solid->numVertices = 0;
    solid->numNormals = 0;
    solid->numCoords = 0;
    solid->numTextures = 1;
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
    solid->textures =
	(SDL_Surface**) malloc(solid->numTextures * sizeof(SDL_Surface*));
    solid->faces = (Face *) malloc(solid->numFaces * sizeof(Face));

    for (i = 0; i < solid->numFaces; i++)
	solid->faces[i].texture = 0;
    
    if ((solid->textures[0] = SDL_LoadBMP("textures/white.bmp")) == NULL) {
	fprintf(stderr, "error loading texture: white.bmp\n");
	exit(EXIT_FAILURE);
    }
    rewind(file);
    while (fscanf(file, "%s", str) != EOF) {
	if (strcmp(str, "v") == 0) {
	    if (fscanf
		(file, "%f %f %f\n", &solid->vertices[a].y,
		 &solid->vertices[a].z, &solid->vertices[a].x) != 3) {
		fprintf(stderr, "Vertices: ");
		return -1;
	    }
	    a++;
	} else if (strcmp(str, "vn") == 0) {
	    if (fscanf
		(file, "%f %f %f\n", &solid->normals[b].y,
		 &solid->normals[b].z, &solid->normals[b].x) != 3) {
		fprintf(stderr, "Normals: ");
		return -1;
	    }
	    b++;
	} else if (strcmp(str, "vt") == 0) {
	    if (fscanf
		(file, "%f %f\n", &solid->coords[c].x,
		 &solid->coords[c].y) != 2) {
		fprintf(stderr, "Texture coordinates: ");
		return -1;
	    }
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
		fprintf(stderr, "Faces enumeration: ");
		return -1;
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

    fclose(file);
    return 0;
}

void equation(float s, float t, Point *p)
{
    //sphere
    //setPoint(p, sin(s) * cos(t), cos(s) * cos(t), sin(t));
    
    //setPoint(p, s, t, 1 / (1 + s*s + t*t));

    //Möbius
    //setPoint(p,
    //         (2 + t * cos(s)) * cos(2*s),
    //	       (2 + t * cos(s)) * sin(2*s),
    //	        t * sin(s));

    // torus
    setPoint(p,
	     sin(s) * (2 + cos(t)),
	     cos(s) * (2 + cos(t)),
	     -sin(t));

    //paraboloid
    //setPoint(p, s, t, t*t - s*s);
}

void equationSolid(Solid *solid,
		   float minS, float maxS, int precisionS,
		   float minT, float maxT, int precisionT)
{
    int p, f = 0;
    float s = minS, t = minT;
    float ds = (maxS - minS) / (precisionS - 1);
    float dt = (maxT - minT) / (precisionT - 1);
    Point normal;

    solid->numVertices = precisionS * precisionT;
    solid->numNormals = solid->numVertices;
    solid->numCoords = 4;
    solid->numTextures = 1;
    solid->numFaces = 2 * (precisionS - 1) * (precisionT - 1);

    solid->vertices = malloc(solid->numVertices * sizeof(Point));
    solid->normals = malloc(solid->numNormals * sizeof(Point));
    solid->coords = malloc(solid->numCoords * sizeof(Texture));
    solid->textures = malloc(solid->numTextures * sizeof(SDL_Surface*));
    solid->faces = malloc(solid->numFaces * sizeof(Face));

    setTexture(&solid->coords[0], 0., 0.);
    setTexture(&solid->coords[1], 0., 1.);
    setTexture(&solid->coords[2], 1., 0.);
    setTexture(&solid->coords[3], 1., 1.);

    if ((solid->textures[0] = SDL_LoadBMP("white.bmp")) == NULL) {
	fprintf(stderr, "error loading texture: white.bmp\n");
	exit(EXIT_FAILURE);
    }

    for (p = 0; p < solid->numVertices; p++) {
	equation(s, t, &solid->vertices[p]);
	Point tt, uu, vv, aa;
	equation(s - ds, t, &tt);
	equation(s + ds, t, &uu);
	diffPoint(&tt, &uu, &vv);
	equation(s, t + dt, &tt);
	equation(s, t - dt, &uu);
	diffPoint(&tt, &uu, &tt);
	vectorProduct(&vv, &tt, &normal);
	//solid->normals[p] = diffPoint(equation(s, t + dt), equation(s, t - dt));

	if (normPoint(&normal) < EPSILON) {
	    //solid->normals[p] = diffPoint(equation(s, t), equation(s, t + dt));
	    diffPoint(&uu, &tt, &tt);
	    equation(s + ds, t + dt, &uu);
	    equation(s - ds, t - dt, &vv);
	    diffPoint(&uu, &vv, &vv);
	    vectorProduct(&vv ,&tt, &aa);
	    normalizePoint(&aa, &solid->normals[p]);
	}
	else
	    normalizePoint(&normal, &solid->normals[p]);
	
	/*solid->normals[p] = vectorProduct(sumPoint(diffPoint(equation(s, t + dt),
	  equation(s, t - dt)),
	  diffPoint(equation(s + ds, t + dt), equation(s - ds, t - dt))),
	  sumPoint(diffPoint(equation(s + ds, t), equation(s - ds, t)),
	  diffPoint(equation(s - ds, t + dt), equation(s + ds, t - dt)))); */
	//solid->normals[p] = diffPoint(equation(s - ds, t), equation(s + ds, t));

	if (p % precisionS != precisionS - 1)
	    s += ds;
	else {
	    s = minS;
	    t += dt;
	}
	//solid->normals[p] = vectorProduct(diffPoint(solid->vertices[p - 1],
	// solid->vertices[p + 1]),
	// diffPoint(solid->vertices[p - precisionS],
	// solid->vertices[p + precisionS]));
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

	    solid->faces[f].texture = 0;
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

	    solid->faces[f].texture = 0;
	    f++;
	}
	p++;
    }
}

void freeSolid(Solid *solid)
{
    int i;
    for (i = 0; i < solid->numTextures; i++)
	SDL_FreeSurface(solid->textures[i]);

    free(solid->vertices);
    free(solid->normals);
    free(solid->coords);
    free(solid->textures);
    free(solid->faces);
}

void drawSolid(const Solid * solid)
{
    int i;
    for (i = 0; i < solid->numFaces; i++)
	projectTriangle(&solid->vertices[solid->faces[i].vertices[0].point],
			&solid->vertices[solid->faces[i].vertices[1].point],
			&solid->vertices[solid->faces[i].vertices[2].point],
			solid->textures[solid->faces[i].texture],
			&solid->coords[solid->faces[i].vertices[0].coord],
			&solid->coords[solid->faces[i].vertices[1].coord],
			&solid->coords[solid->faces[i].vertices[2].coord],
			&solid->normals[solid->faces[i].vertices[0].normal],
			&solid->normals[solid->faces[i].vertices[1].normal],
			&solid->normals[solid->faces[i].vertices[2].normal]);
}
