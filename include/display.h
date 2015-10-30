#ifndef DISPLAY_H
#define DISPLAY_H

#include "color.h"
#include "coord.h"

extern void (*initDisplay)(int screenWidth, int screenHeight, 
			   const Color *background, const Color *untextured);
extern void (*resizeDisplay)(int screenWidth, int screenHeight);
extern void (*resetDisplay)();
extern void (*pixelDisplay)(const Coord *A, const Color *color);
extern void (*blitDisplay)();
extern int (*getWidthDisplay)();
extern int (*getHeightDisplay)();
extern void (*freeDisplay)();

#endif //DISPLAY_H
