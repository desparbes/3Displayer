#include "SDL/SDL.h"
#include "coord.h"
#include "scene.h"
#include "color.h"

void setPixel(const Coord *A, const Color *color)
{
    SDL_Surface *s = getScreen();
    if (A->w >= 0 && A->w < s->w && A->h >= 0 && A->h < s->h) {
	Uint32 pixel = SDL_MapRGB(s->format, color->r, color->g, color->b);
	*((Uint32 *) (s->pixels) + A->w + A->h * s->w) = pixel;
    }
}
