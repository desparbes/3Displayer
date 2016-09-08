#ifndef DRAW_H
#define DRAW_H

#include "point.h"
#include "coord.h"
#include "color.h"
#include "position.h"
#include "texture.h"
#include "lens.h"
#include "pixel.h"

void drawPixel(Lens *l, Coord A, float depthA, Color color);

void drawSegment(Lens *l, Coord A, Coord B,
                 float depthA, float depthB, Color color);
void drawTriangle(Lens *l, Texture *triangle, Pixel A, Pixel B, Pixel C);
 
#endif //DRAW_H
