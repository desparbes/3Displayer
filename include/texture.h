#ifndef TEXTURE_H
#define TEXTURE_H

#include "position.h"
#include "color.h"

typedef struct Texture Texture;

Texture *loadTexture(const char *fileName);
Color getPixelTexture(const Texture *texture, const Position *p);
void freeTexture(Texture *texture);

#endif //TEXTURE_H
