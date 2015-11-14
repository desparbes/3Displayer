#include <stdio.h>
#include <stdlib.h>

#include "position.h"
#include "color.h"
#include "SDL/SDL.h"

typedef struct Texture {
    SDL_Surface *t;
} Texture;

Texture *loadTexture(const char *fileName)
{
    SDL_Surface *tmp = SDL_LoadBMP(fileName);
    if (!tmp)
	return NULL;
    Texture *texture = malloc(sizeof(Texture));
    texture->t = tmp;
    return texture;
}

void getPixelTexture(const Texture *texture, const Position *p, Color *c)
{
    Uint32 *pixel = (Uint32 *) 
		     (texture->t->pixels +
		      (int) (p->x * texture->t->w) * 
		      texture->t->format->BitsPerPixel / 8 +
		      (int) (p->y * texture->t->h) *
		      texture->t->w *
		      texture->t->format->BitsPerPixel / 8);
    SDL_GetRGB(*pixel, texture->t->format, &c->r, &c->g, &c->b);
}

void freeTexture(Texture *texture)
{
    if (texture) {
	SDL_FreeSurface(texture->t);
	free(texture);
    }
}
