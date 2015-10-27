#ifndef TEXTURE_H
#define TEXTURE_H

#include "position.h"
#include "color.h"

typedef struct Texture Texture;

extern Texture *(*loadTexture)(const char *fileName);
extern void (*getPixelTexture)(const Texture *texture, 
			       const Position *p, Color *c);
extern void (*freeTexture)(Texture *texture);

#endif //TEXTURE_H
