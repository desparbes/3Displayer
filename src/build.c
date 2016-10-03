#include <stdio.h>
#include <stdlib.h>

#include "build.h"

void setSegmentBuild(Segment *u, int A, int B)
{
    if (A < B) {
        u->A = A;
        u->B = B;
    } else {
        u->A = B;
        u->B = A;
    }
}

int compareSegmentBuild(const void *u, const void *v)
{
    Segment a = *(const Segment *) u;
    Segment b = *(const Segment *) v;
    int diff = a.A - b.A;
    return diff == 0 ? a.B - b.B : diff;
}

void formatSegmentBuild(Solid *solid)
{
    Segment *tmp = malloc(solid->numSegments * sizeof(Segment));
    int j = 0;
    if (solid->numSegments > 0) {
        tmp[0] = solid->segments[0];
        j++;
    }
    for (int i = 1; i < solid->numSegments; i++) {
        if (compareSegmentBuild(&solid->segments[i - 1], &solid->segments[i])) {
            tmp[j] = solid->segments[i];
            j++;
        }
    }
    solid->numSegments = j;
    free(solid->segments);
    solid->segments = realloc(tmp, j * sizeof(Segment));
}

void addSegmentBuild(Solid *solid, const Segment *segment, int *bufferSize)
{
    if(solid->numSegments >= *bufferSize){
        *bufferSize *= 2;
        solid->segments = realloc(solid->segments,
                        	  *bufferSize * sizeof(Segment));
    }
    solid->segments[solid->numSegments++] = *segment;
}
