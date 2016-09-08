#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include "position.h"
#include "texture.h"
#include "color.h"
#include "SDL.h"

struct Texture {
    SDL_Surface *surface;
};

Texture *loadTexture(char const *fileName)
{
    SDL_Surface *tmp = SDL_LoadBMP(fileName);
    if (!tmp)
	return NULL;
    Texture *texture = g_malloc0(sizeof*texture);
    texture->surface = tmp;
    return texture;
}

#define PIXEL_OFFSET(format, w, h, x, y)                        \
    (((format)->BitsPerPixel/8) * ((int)(x)*(w) + (int)(y)*(h)*(w)))

#define SURFACE_PIXEL_OFFSET(s, x, y)                   \
    PIXEL_OFFSET((s)->format, (s)->w, (s)->h, x, y)

void getPixelTexture(Texture const *texture, Position const *p, Color *c)
{
    Uint32 *pixel;
    SDL_Surface *s = texture->surface;

    pixel = (Uint32*)s->pixels + SURFACE_PIXEL_OFFSET(s, p->x, p->y);
    SDL_GetRGB(*pixel, s->format, &c->r, &c->g, &c->b);
}

void freeTexture(Texture *texture)
{
    if (texture != NULL) {
	SDL_FreeSurface(texture->surface);
	free(texture);
    }
}
