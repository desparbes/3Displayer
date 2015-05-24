#ifndef COORD_H
#define COORD_H

typedef struct Coord {
    int w;
    int h;
} Coord;

void setCoord(Coord *c, int w, int h);
void diffRect(const Coord *A, const Coord *B, Coord *S);
int relativeProduct(const Coord *u, const Coord *v);
float distanceCoord(const Coord * A, const Coord * B);

#endif // COORD_H
