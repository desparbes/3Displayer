#include <stdio.h>
#include <stdlib.h>

#include "position.h"
#include "color.h"
#include "ncurses.h"

typedef struct Texture Texture;

Texture *loadTexture_(const char *fileName)
{
    return NULL;
}

void getPixelTexture_(const Texture *texture, const Position *p, Color *c) {}

void freeTexture_(Texture *texture) {}

Texture *(*loadTexture)(const char *) = &loadTexture_;
void (*getPixelTexture)(const Texture *, const Position *, Color *) = 
    &getPixelTexture_;
void (*freeTexture)(Texture *) = &freeTexture_;
