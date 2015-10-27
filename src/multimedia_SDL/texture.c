#include <stdio.h>
#include <stdlib.h>

#include "position.h"
#include "color.h"
#include "SDL/SDL.h"

typedef struct Texture {
    SDL_Surface *t;
} Texture;

Texture *loadTexture_(const char *fileName)
{
    SDL_Surface *tmp = SDL_LoadBMP(fileName);
    if (!tmp) {
	fprintf(stderr, "%s\n", SDL_GetError());
	return NULL;
    }
    Texture *texture = malloc(sizeof(Texture));
    texture->t = tmp;
    return texture;
}

void getPixelTexture_(const Texture *texture, const Position *p, Color *c)
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

void freeTexture_(Texture *texture)
{
    if (texture) {
	SDL_FreeSurface(texture->t);
	free(texture);
    }
}
