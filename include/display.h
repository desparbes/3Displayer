#ifndef DISPLAY_H
#define DISPLAY_H

#include "color.h"
#include "coord.h"

extern void (*initDisplay)(int screenWidth, int screenHeight, 
			   const Color *background);
extern void (*resetDisplay)();
extern void (*pixelDisplay)(const Coord *A, const Color *color);
extern void (*blitDisplay)();
extern void (*freeDisplay)();

#endif //DISPLAY_H
