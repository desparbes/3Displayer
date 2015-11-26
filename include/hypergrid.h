#ifndef HYPERGRID_H
#define HYPERGRID_H

int getNumVertices(int *precision, int size);
int getNumNormals(int *precision, int size);
int getNumSegments(int *precision, int size);
int getNumFaces(int *precision, int size);
void getGridFromId(int id, int size, int *precision, int *grid, int dim);
int getIdFromGrid(int *precision, int *grid, int dim);

#endif // HYPERGRID_H
