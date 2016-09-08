#include <glib/gi18n.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include "coord.h"
#include "color.h"
#include "window.h"

#define SDL_WINDOW(win)  ((_3Displayer_SDL_Window*)(win))


bool PollEvent(Window *window, Event *event);
typedef struct _3Displayer_SDL_Window_ _3Displayer_SDL_Window;

struct _3Displayer_SDL_Window_ {
    Window parent;
    SDL_Window *window;
    SDL_Surface *surface;
    Uint32 background;
};

static Window _win_ops;

static void
sdl_window_resize(Window *w, int width, int height)
{
    SDL_SetWindowSize(SDL_WINDOW(w)->window, width, height);
}

Window* CreateWindow(int width, int height, Color background)
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
	fprintf(stderr, _("SDL_Init error: %s"), SDL_GetError());
        return NULL;
    }
    _3Displayer_SDL_Window *w;
    w = malloc(sizeof*w);
    w->window = SDL_CreateWindow(
        "3Displayer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS);
    if (w->window == NULL) {
        fprintf(stderr, _("Could not create window: %s"), SDL_GetError());
        free(w);
        return NULL;
    }
    w->surface = SDL_GetWindowSurface(w->window);
    w->background = SDL_MapRGB(
        w->surface->format, background.r, background.g, background.b);
    w->parent = _win_ops;
    return (Window*) w;
}

static void
sdl_window_reset(Window *w)
{
    SDL_FillRect(SDL_WINDOW(w)->surface, NULL, SDL_WINDOW(w)->background);
}

static void
sdl_window_update(Window *w)
{
    SDL_UpdateWindowSurface(SDL_WINDOW(w)->window);
}

static int
sdl_window_getWidth(Window *w)
{
    return SDL_WINDOW(w)->surface->w;
}

static int
sdl_window_getHeight(Window *w)
{
    return SDL_WINDOW(w)->surface->h;
}

static void
sdl_window_free(Window *w)
{
    SDL_DestroyWindow(SDL_WINDOW(w)->window);
}

static void
sdl_window_set_pixel(Coord pos, Color col)
{
    
}

static Window _win_ops = {
    .resize = sdl_window_resize,
    .reset = sdl_window_reset,
    .update = sdl_window_update,
    .getWidth = sdl_window_getWidth,
    .getHeight = sdl_window_getHeight,
    .free = sdl_window_free,
    .pollEvent = PollEvent,
    .setPixel = sdl_window_set_pixel,
    .setPixel = sdl_window_get_pixel,
};
