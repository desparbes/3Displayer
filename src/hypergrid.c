#include <stdio.h>
#include <stdlib.h>

static int powi(int n, int dim)
{
    if (dim < 0)
	return 0;

    int r = 1;
    for (int i = 0; i < dim; i++)
	r *= n;
    return r;
}

int getNumVertices(int *precision, int size)
{
    int r = 1;
    for (int i = 0; i < size; i++) {
	r *= precision[i];
    }
    return r;
}

int getNumNormals(int *precision, int size)
{
    int r = 0;
    for (int i = 0; i < size - 1; i++) {
	for (int j = i + 1; j < size; j++) {
	    r += precision[i] * precision[j];
	}
    }
    return powi(2, size - 2) * r;
}

int getNumSegments(int *precision, int size)
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

int getNumFaces(int *precision, int size)
{
    int r = 0;
    for (int i = 0; i < size - 1; i++) {
	for (int j = i + 1; j < size; j++) {
	    r += (precision[i] - 1) * (precision[j] - 1);
	}
    }
    return powi(2, size - 2) * r;
}

void getGridFromId(int id, int size, int *precision, int *grid, int dim)
{
    for (int i = dim - 1; i >= 0; i--) {
	size /= precision[i];
	grid[i] = id / size;
	id -= grid[i] * size;
    }
}


int getIdFromGrid(int *precision, int *grid, int dim)
{
    int scale = 1;
    int r = 0;
    for (int i = 0; i < dim; i++) {
	r += scale * grid[i];
	scale *= precision[i];
    }
    return r;
}
