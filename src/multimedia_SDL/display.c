#include <stdio.h>
#include <stdlib.h>

#include "SDL/SDL.h"
#include "coord.h"
#include "color.h"

static struct {
    SDL_Surface *screen;
    Uint32 background;
    Color untextured;
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

void initDisplay_(int screenWidth, int screenHeight, const Color *background,
    const Color *untextured)
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
	fprintf(stderr, "Error SDL_Init: %s", SDL_GetError());
	exit(EXIT_FAILURE);
    }
    resizeDisplay_(screenWidth, screenHeight);
    display.untextured = *untextured;
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

void getPixelDisplay_(const Coord *A, Color *color)
{
    SDL_Surface *s = display.screen;
    if (A->w >= 0 && A->w < s->w && A->h >= 0 && A->h < s->h) {
	Uint32 pixel = *((Uint32 *) (s->pixels) + A->w + A->h * s->w);
	SDL_GetRGB(pixel, s->format, &color->r, &color->g, &color->b);
    }
}

void setPixelDisplay_(const Coord *A, const Color *color)
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

void getUntexturedDisplay_(Color *c)
{
    *c = display.untextured;
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

void (*initDisplay)(int, int, const Color *, const Color *) = &initDisplay_;
void (*resizeDisplay)(int, int) = &resizeDisplay_;
void (*resetDisplay)() = &resetDisplay_;
void (*getPixelDisplay)(const Coord *, Color *) = &getPixelDisplay_;
void (*setPixelDisplay)(const Coord *, const Color *) = &setPixelDisplay_;
void (*blitDisplay)() = &blitDisplay_;
void (*getUntexturedDisplay)(Color *) = &getUntexturedDisplay_;
int (*getWidthDisplay)() = &getWidthDisplay_;
int (*getHeightDisplay)() = &getHeightDisplay_;
void (*freeDisplay)() = &freeDisplay_;
