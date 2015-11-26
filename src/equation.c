#include <stdio.h>
#include <stdlib.h>

#include "build.h"
#include "position.h"
#include "texture.h"
#include "point.h"
#include "parametric.h"

typedef struct Grid {
    int grid[3];
} Grid;

static int minimum(int a, int b)
{
    return a < b ? a : b;
}

static int powi(int n, int dim)
{
    if (dim < 0)
	return 0;

    int r = 1;
    for (int i = 0; i < dim; i++)
	r *= n;
    return r;
}

static int getNumVertices(int *precision, int size)
{
    int r = 1;
    for (int i = 0; i < size; i++) {
	r *= precision[i];
    }
    return r;
}

static int getNumNormals(int *precision, int size)
{
    int r = 0;
    for (int i = 0; i < size - 1; i++) {
	for (int j = i + 1; j < size; j++) {
	    r += precision[i] * precision[j];
	}
    }
    return 2 * powi(2, size - 2) * r;
}

static int getNumSegments(int *precision, int size)
{
    int s = 0;
    for (int i = 0; i < size; i++) {
	int p = (precision[i] - 1);
	for (int j = 1; j < size; j++)
	    p *= precision[(i + j) % size];
	s += p;
    }
    return s;
}

static int getNumFaces(int *precision, int size)
{
    int r = 0;
    for (int i = 0; i < size - 1; i++) {
	for (int j = i + 1; j < size; j++) {
	    r += (precision[i] - 1) * (precision[j] - 1);
	}
    }
    return 4 * powi(2, size - 2) * r;
}

static void getGridFromId(int id, int size, int *precision, int *grid, int dim)
{
    for (int i = dim - 1; i >= 0; i--) {
	size /= precision[i];
	grid[i] = id / size;
	id -= grid[i] * size;
    }
}

static int getIdFromGrid(int *precision, int *grid, int dim)
{
    int r = grid[0];
    for (int i = 1; i < dim; i++)
	r += precision[i - 1] * grid[i];
    return r;
}

static void addGrid(int *A, int x, int y, int z, int dim, int *result)
{
    dim >= 1 ? (result[0] = A[0] + x) : (result[0] = 0); 
    dim >= 2 ? (result[1] = A[1] + y) : (result[1] = 0); 
    dim >= 3 ? (result[2] = A[2] + x) : (result[2] = 0); 
}

static int isInRangeGrid(int *A, int *precision, int dim)
{
    for (int i = 0; i < dim; i++) {
	if (A[i] < 0 || A[i] >= precision[i])
	    return 0;
    }
    return 1;
}

static int isOnBorderGrid(int *A, int *precision, int dim)
{
    for (int i = 0; i < dim; i++) {
	if (A[i] == 0 || A[i] == precision[i] - 1)
	    return 1;
    }
    return 0;
}

void setNormal(Solid *solid, int **normalBuffer, int O, int A, int B, int dim)
{
    static int n = 0;
    Point u, v, normal;
    diffPoint(&solid->vertices[A], &solid->vertices[O], &u);
    diffPoint(&solid->vertices[B], &solid->vertices[O], &v);
    pointProduct(&u, &v, &normal);
    normalizePoint(&normal, &normal);

    normalBuffer[O][dim] = n;
    solid->normals[n] = normal;
    n++;

    normalBuffer[O][3 + dim] = n;
    setPoint(&solid->normals[n], -normal.x, -normal.y, -normal.z);
    n++;
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

static void setSquare(Solid *solid, int **normalBuffer, 
		      int a, int b, int c, int d, int dim)
{
    static int f = 0;
    
    setPointFace(solid, f, a, b, c);
    setPositionFace(solid, f, 1, 3, 2);
    setNormalFace(solid, f, 
		  normalBuffer[a][dim], 
		  normalBuffer[b][dim], 
		  normalBuffer[c][dim]);
    f++;

    setPointFace(solid, f, a, c, d);
    setPositionFace(solid, f, 1, 2, 0);
    setNormalFace(solid, f, 
		  normalBuffer[a][3 + dim], 
		  normalBuffer[b][3 + dim], 
		  normalBuffer[c][3 + dim]);
    f++;
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
/*
int calculateBottom(int *precision, int nbInput)
{
    if (nbInput >= 2)
	return (precision[0] - 1) * (precision[1] - 1);
    return 0;
}

int calculateSide(int *precision, int nbInput)
{
    if (nbInput >= 3)
	return (precision[1] - 1) * (precision[2] - 1);
    return 0;
}

int calculateFront(int *precision, int nbInput)
{
    if (nbInput >= 3)
	return (precision[2] - 1) * (precision[0] - 1);
    return 0;
}
*/


Solid *loadEquation(const char *eqName, const char *bmpName)
{
    float *min = NULL, *max = NULL;
    int *precision = NULL;
    int nbInput;
    int nbOutput;
    
    if (!initEquation(&min, &max, &precision, &nbInput, &nbOutput, eqName)) {
	fprintf(stderr, "Error loading equation\n");
	return NULL;
    }

    Solid *solid = malloc(sizeof(Solid));
    float *input = malloc(nbInput * sizeof(float));
    float *output = malloc(nbOutput * sizeof(float));
    float *interval = malloc(nbInput * sizeof(float));
    int dim = minimum(nbInput, 3);
    int a, e = 0;
    for (int i = 0; i < nbInput; i++) {
	input[i] = min[i];
	interval[i] = (max[i] - min[i]) / (precision[i] - 1);
    } 

    solid->numVertices = getNumVertices(precision, dim);
    solid->numSegments = getNumSegments(precision, dim);
    solid->numCoords = 4;    
    solid->numNormals = getNumNormals(precision, dim);
    solid->numFaces = getNumFaces(precision, dim);

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
	normalBuffer[i] = malloc(6 * sizeof(int));

    setPosition(&solid->coords[0], 0., 0.);
    setPosition(&solid->coords[1], 0., 1.);
    setPosition(&solid->coords[2], 1., 0.);
    setPosition(&solid->coords[3], 1., 1.);

    for (a = 0; a < solid->numVertices; a++) {
	getGridFromId(a, solid->numVertices, precision, 
		      gridBuffer[a].grid, dim);
	updateInput(input, min, interval, gridBuffer[a].grid, dim);
	getValueFromEquation(input, output);
	mapOutput(output, &solid->vertices[a], nbOutput);
    }
    
    // Calcul des normales
    for (a = 0; a < solid->numVertices; a++) {
        int *A = gridBuffer[a].grid;
	if (isOnBorderGrid(A, precision, dim)) {
	    for (int i = 0; i < dim - 1; i++) {
		for (int j = i + 1; j < dim; j++) {
		    int B[3];
		    int C[3];
		    int D[3];
		    int opposed[3];

		    addGrid(A, 
			    i == 0 || j == 0,
			    i == 1 || j == 1, 
			    i == 2 || j == 2, dim, C);
		    addGrid(A, 
			    -(i == 0 || j == 0),
			    -(i == 1 || j == 1), 
			    -(i == 2 || j == 2), dim, opposed);
		    if (isInRangeGrid(C, precision, dim)) {
			addGrid(A, 
				i != 1 && j != 1, 
				i != 2 && j != 2, 
				i != 0 && j != 0, dim, B);
			addGrid(A, 
				i != 2 && j != 2, 
				i != 0 && j != 0, 
				i != 1 && j != 1, dim, B);
		    } else if (isInRangeGrid(opposed, precision, dim)) {
			addGrid(A, 
				-(i != 1 && j != 1), 
				-(i != 2 && j != 2), 
				-(i != 0 && j != 0), dim, B);
			addGrid(A, 
				-(i != 2 && j != 2), 
				-(i != 0 && j != 0), 
				-(i != 1 && j != 1), dim, B);
		    } else {
			printf("break: a: %d, i: %d, j: %d, C: (%d, %d, %d), opposed: (%d, %d, %d)\n", a, i, j, C[0], C[1], C[2], opposed[0], opposed[1], opposed[2]);
			break;
		    }
		    int b = getIdFromGrid(precision, B, dim);
		    int d = getIdFromGrid(precision, D, dim);
		    setNormal(solid, normalBuffer, a, b, d, i);
		    setNormal(solid, normalBuffer, a, d, b, i);
		/*printf("%d: (%d, %d, %d)\n", 
		       a,
		       gridBuffer[a].grid[0], 
		       gridBuffer[a].grid[1],
		       gridBuffer[a].grid[2]);
		*/
		}
	    }
	}
    }
    printf("numNormals: %d\n", solid->numNormals);
    for (int i = 0; i < solid->numVertices; i++) {
	for (int j = 0; j < 6; j++) {
	    printf("i: %d / numVertices, j: %d / 6, normal: %d\n",
		   i, j,
		   normalBuffer[i][j]);
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
	    /*
	    //Si p est sur la bordure pour la dimension i
	    if (gridBuffer[p].grid[i] == 0 || gridBuffer[p].grid[i] == (precision[i] - 1)) {
		if (gridBuffer[p].grid[i] == 0 || gridBuffer[p].grid[i] == (precision[i] - 1)) {
	    }
	    */
	    delta *= precision[i];
	}
	

/*
	addGrid(&gridBuffer[p], 0, 0, 1, &A);
	addGrid(&gridBuffer[p], 0, 1, 0, &C);
*/
	int *A = gridBuffer[a].grid;
	for (int i = 0; i < 3; i++) {
	    int C[3];
	    addGrid(A, i != 0, i != 1, i != 2, dim, C);
	    if (isInRangeGrid(C, precision, dim) && 
		(A[i] == 0 || A[i] == precision[i] - 1)) {
		int B[3];
		int D[3];
		addGrid(A, i == 1, i == 2, i == 0, dim, B);
		addGrid(A, i == 2, i == 0, i == 1, dim, D);
		int b = getIdFromGrid(precision, B, dim);
		int c = getIdFromGrid(precision, C, dim);
		int d = getIdFromGrid(precision, D, dim);
		setSquare(solid, normalBuffer, a, b, c, d, i);		    
	    }
	}
    }

/*
	addGrid(&A, 0, 1, 1, &C);
	if (isInRangeGrid(&C)) {
	    if (A.grid[0] == 0)
		setFace(solid, p, getIdFromGrid(&C));
	    if (A.grid[0] == precision[0] - 1)
		setFace(solid, getIdFromGrid(&C), p);
	}
	addGrid(&A, 1, 0, 1, &C);
	if (isInRangeGrid(&C)) {
	    if (A.grid[1] == 0)
		setFace(solid, p, getIdFromGrid(&C));
	    if (A.grid[1] == precision[1] - 1)
		setFace(solid, getIdFromGrid(&C), p);
	}
	addGrid(&A, 1, 1, 0, &C);
	if (isInRangeGrid(&C)) {
	    if (A.grid[2] == 0)
		setFace(solid, p, getIdFromGrid(&C));
	    if (A.grid[2] == precision[2] - 1)
		setFace(solid, getIdFromGrid(&C), p);
       }

    int bottom = calculateBottom(precision, nbInput);
    for (p = 0; p < bottom; p++) {
	setFace(solid, f, p, p + 1, p + precision[0],
	solid->faces[f].vertices[0].point = p;
	solid->faces[f].vertices[1].point = p + 1;
	solid->faces[f].vertices[2].point = p + precisionS;
	solid->faces[f].vertices[0].normal = p;
	solid->faces[f].vertices[1].normal = p + 1;
	solid->faces[f].vertices[2].normal = p + precisionS;
	solid->faces[f].vertices[0].coord = 1;
	solid->faces[f].vertices[1].coord = 3;
	solid->faces[f].vertices[2].coord = 0;
 
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

	    HANDLE(getValueFromEquation(x, y, z, s, t + dt, A))
	    solid->segments[e].A = p; 
	    solid->segments[e].B = p + precisionS; 	    
	    e++;

	    s = minS;
	    t += dt;
	} else if (p >= (solid->numVertices - precisionS)) { // north
	    A = &solid->vertices[p - precisionS];
	    B = &solid->vertices[p + 1];

	    solid->segments[e].A = p; 
	    solid->segments[e].B = p + 1;	    
	    e++;

	    s += ds;
	} else { //elsewhere
	    A = &solid->vertices[p + 1];
	    B = &solid->vertices[p + precisionS];
	    
	    if (p < precisionS) // south
		HANDLE(getValueFromEquation(x, y, z, s + ds, t, A))
	    HANDLE(getValueFromEquation(x, y, z, s, t + dt, B))
	    solid->segments[e].A = p; 
	    solid->segments[e].B = p + 1; 
	    e++;
	    solid->segments[e].A = p; 
	    solid->segments[e].B = p + precisionS; 
	    e++;
	    solid->segments[e].A = p; 
	    solid->segments[e].B = p + precisionS + 1; 
	    e++;

	    s += ds;
	}
	diffPoint(A, O, &u);
	diffPoint(B, O, &v);
	pointProduct(&u, &v, normal);
	normalizePoint(normal, normal);
    }

    for (p = 0; f < solid->numFaces; p++) {
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
    }
    */
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



