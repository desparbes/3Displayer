#include <stdio.h>
#include <stdlib.h>

#include "SDL/SDL.h"
#include "coord.h"
#include "color.h"

static struct {
    SDL_Surface *screen;
    Uint32 background;
} display;

void initDisplay_(int screenWidth, int screenHeight, const Color *background)
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
	fprintf(stderr, "Error SDL_Init: %s", SDL_GetError());
	exit(EXIT_FAILURE);
    }
    if ((display.screen = SDL_SetVideoMode(screenWidth, screenHeight, 
					   32, SDL_HWSURFACE | SDL_DOUBLEBUF)) 
	== NULL) {
	fprintf(stderr, "Error SDL_SetVideoMode: %s", SDL_GetError());
	exit(EXIT_FAILURE);
    }
    display.background = SDL_MapRGB(display.screen->format, 
				    background->r, 
				    background->g, 
				    background->b);
    SDL_WM_SetCaption("3Displayer", NULL);
    SDL_EnableKeyRepeat(1, 10);
}

void resetDisplay_()
{
    SDL_FillRect(display.screen, NULL, display.background);
}

void pixelDisplay_(const Coord *A, const Color *color)
{
    SDL_Surface *s = display.screen;
    if (A->w >= 0 && A->w < s->w && A->h >= 0 && A->h < s->h) {
	Uint32 pixel = SDL_MapRGB(s->format, color->r, color->g, color->b);
	*((Uint32 *) (s->pixels) + A->w + A->h * s->w) = pixel;
    }
}

void blitDisplay_()
{
    SDL_Flip(display.screen);
}

void freeDisplay_()
{
    SDL_FreeSurface(display.screen);
    SDL_Quit();
}

