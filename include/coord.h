#ifndef COORD_H
#define COORD_H

typedef struct Coord {
    int w;
    int h;
} Coord;

void setCoord(Coord *c, int w, int h);
void translateCoord(Coord *c, int w, int h);
void diffCoord(const Coord *A, const Coord *B, Coord *S);
void sumCoord(const Coord *A, const Coord *B, Coord *S);
int productCoord(const Coord *u, const Coord *v);
float distanceCoord(const Coord * A, const Coord * B);

#endif // COORD_H
