#include "position.h"
#include "color.h"

typedef struct Texture Texture;

Texture *loadTexture(char *fileName);
void getPixelTexture(const Texture *texture, const Position *p, Color *c);
void freeTexture(Texture *texture);
