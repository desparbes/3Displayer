#ifndef TEXTURE_H
#define TEXTURE_H

#include "position.h"
#include "color.h"

typedef struct Texture Texture;

Texture *loadTexture(const char *fileName);
void getPixelTexture(const Texture *texture, const Position *p, Color *c);
void freeTexture(Texture *texture);

#endif //TEXTURE_H
