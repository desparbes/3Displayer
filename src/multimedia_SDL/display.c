#include <stdio.h>
#include <stdlib.h>

#include "SDL/SDL.h"
#include "coord.h"
#include "color.h"

static struct {
    SDL_Surface *screen;
    Uint32 background;
} display;

void resizeDisplay_(int screenWidth, int screenHeight)
{
    if ((display.screen = SDL_SetVideoMode(screenWidth, screenHeight, 32, 
					   SDL_HWSURFACE | SDL_DOUBLEBUF | 
					   SDL_RESIZABLE)) 
	== NULL) {
	fprintf(stderr, "Error SDL_SetVideoMode: %s", SDL_GetError());
	exit(EXIT_FAILURE);
    }
}

void initDisplay_(int screenWidth, int screenHeight, const Color *background)
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
	fprintf(stderr, "Error SDL_Init: %s", SDL_GetError());
	exit(EXIT_FAILURE);
    }
    resizeDisplay_(screenWidth, screenHeight);
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

int getWidthDisplay_()
{
    return display.screen->w;
}

int getHeightDisplay_()
{
    return display.screen->h;
}

void freeDisplay_()
{
    SDL_FreeSurface(display.screen);
    SDL_Quit();
}

void (*initDisplay)(int, int, const Color *) = &initDisplay_;
void (*resizeDisplay)(int, int) = &resizeDisplay_;
void (*resetDisplay)() = &resetDisplay_;
void (*pixelDisplay)(const Coord *, const Color *) = &pixelDisplay_;
void (*blitDisplay)() = &blitDisplay_;
int (*getWidthDisplay)() = &getWidthDisplay_;
int (*getHeightDisplay)() = &getHeightDisplay_;
void (*freeDisplay)() = &freeDisplay_;
