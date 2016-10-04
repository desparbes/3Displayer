#include <glib/gi18n.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include "coord.h"
#include "color.h"
#include "window.h"

#define SDL_BACKEND(win)  ((sdl_backend*)(win))


void sdl_window_waitevent(Window *window, Event *event);
bool sdl_window_pollevent(Window *window, Event *event);

typedef struct sdl_backend_ sdl_backend;

struct sdl_backend_ {
    Window parent;
    SDL_Window *window;
    SDL_Renderer *renderer;
    Color bg; // background colour
};

static Window _win_ops;

Window* CreateWindow(int width, int height, Color background)
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        fprintf(stderr, _("SDL_Init error: %s"), SDL_GetError());
        return NULL;
    }
    sdl_backend *w;
    w = malloc(sizeof*w);
    w->window = SDL_CreateWindow(
        "3Displayer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height, SDL_WINDOW_RESIZABLE);
    if (w->window == NULL) {
        fprintf(stderr, _("Could not create window: %s"), SDL_GetError());
        free(w);
        return NULL;
    }
    w->renderer = SDL_CreateRenderer(w->window, -1, SDL_RENDERER_SOFTWARE);
    w->bg = background;
    w->parent = _win_ops;
    return (Window*) w;
}

static void
sdl_window_reset(Window *win_if)
{
    sdl_backend *win = SDL_BACKEND(win_if);
    SDL_SetRenderDrawColor(win->renderer, win->bg.r, win->bg.g, win->bg.b, 255);
    SDL_RenderClear(win->renderer);
}

static void
sdl_window_update(Window *win_if)
{
    sdl_backend *win = SDL_BACKEND(win_if);
    SDL_RenderPresent(win->renderer);
}

static int
sdl_window_getWidth(Window *win_if)
{
    sdl_backend *win = SDL_BACKEND(win_if);
    int width, height;
    SDL_GetWindowSize(win->window, &width, &height);
    return width;
}

static int
sdl_window_getHeight(Window *win_if)
{
    sdl_backend *win = SDL_BACKEND(win_if);
    int width, height;
    SDL_GetWindowSize(win->window, &width, &height);
    return height;
}

static void
sdl_window_free(Window *w)
{
    SDL_DestroyWindow(SDL_BACKEND(w)->window);
}

static void
sdl_window_set_pixel(Window *win_if, Coord pos, Color c)
{
    sdl_backend *win = SDL_BACKEND(win_if);
    SDL_SetRenderDrawColor(win->renderer, c.r, c.g, c.b, 255);
    if (SDL_RenderDrawPoint(win->renderer, pos.w, pos.h) < 0)
        fprintf(stderr, "render draw points failed: %s\n", SDL_GetError());
}

static Color
sdl_window_get_pixel(Window *win_if, Coord p)
{
    sdl_backend *win = SDL_BACKEND(win_if);
    Color c;
    SDL_Rect rect;
    uint32_t pixel;

    rect.x = p.w; rect.y = p.w;
    rect.w = 1; rect.h = 1;

    SDL_RenderReadPixels(win->renderer, &rect, SDL_PIXELFORMAT_RGBA8888, &pixel, 0);
    c.r = (pixel >> 24) & 0xFF;
    c.g = (pixel >> 16) & 0xFF;
    c.b = (pixel >> 8) & 0xFF;
    return c;
}

static Window _win_ops = {
    .reset = sdl_window_reset,
    .update = sdl_window_update,
    .getWidth = sdl_window_getWidth,
    .getHeight = sdl_window_getHeight,
    .free = sdl_window_free,
    .pollEvent = sdl_window_pollevent,
    .waitEvent = sdl_window_waitevent,
    .setPixel = sdl_window_set_pixel,
    .getPixel = sdl_window_get_pixel,
};
