#ifndef DRAW_H
#define DRAW_H

#include "point.h"
#include "coord.h"
#include "color.h"
#include "position.h"
#include "texture.h"
#include "lens.h"
#include "pixel.h"
#include "config.h"


void initRenderer(Config *conf);
void drawPixel(Lens *l, Coord A, float depthA, Color color);
void drawTriangle(Lens *l, Texture *triangle, Pixel A, Pixel B, Pixel C);
void drawSegment(Lens *l, Coord A, Coord B,
                 float depthA, float depthB, Color color);

#endif //DRAW_H
