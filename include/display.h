#ifndef DISPLAY_H
#define DISPLAY_H

#include "color.h"
#include "coord.h"

void initDisplay(int screenWidth, int screenHeight, const Color *background);
void resetDisplay();
void pixelDisplay(const Coord *A, const Color *color);
void blitDisplay();
void freeDisplay();

#endif //DISPLAY_H
