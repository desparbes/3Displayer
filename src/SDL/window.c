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
    SDL_Renderer *renderer;
    Color bg; // background colour
};

static Window _win_ops;

static void
sdl_window_resize(Window *w, int width, int height)
{
    //SDL_SetWindowSize(SDL_WINDOW(w)->window, width, height);
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
    _3Displayer_SDL_Window *win = SDL_WINDOW(win_if);
    SDL_SetRenderDrawColor(win->renderer, win->bg.r, win->bg.g, win->bg.b, 255);
    SDL_RenderClear(win->renderer);
}

static void
sdl_window_update(Window *win_if)
{
    _3Displayer_SDL_Window *win = SDL_WINDOW(win_if);
    SDL_RenderPresent(win->renderer);
}

static int
sdl_window_getWidth(Window *win_if)
{
    _3Displayer_SDL_Window *win = SDL_WINDOW(win_if);
    int width, height;
    SDL_GetWindowSize(win->window, &width, &height);
    return width;
}

static int
sdl_window_getHeight(Window *win_if)
{
    _3Displayer_SDL_Window *win = SDL_WINDOW(win_if);
    int width, height;
    SDL_GetWindowSize(win->window, &width, &height);
    return height;
}

static void
sdl_window_free(Window *w)
{
    SDL_DestroyWindow(SDL_WINDOW(w)->window);
}

static void
sdl_window_set_pixel(Window *win_if, Coord pos, Color c)
{
    _3Displayer_SDL_Window *win = SDL_WINDOW(win_if);
    SDL_SetRenderDrawColor(win->renderer, c.r, c.g, c.b, 255);
    if (SDL_RenderDrawPoint(win->renderer, pos.w, pos.h) < 0)
        fprintf(stderr, "render draw points failed: %s\n", SDL_GetError());
}

static Color
sdl_window_get_pixel(Window *win_if, Coord p)
{
    _3Displayer_SDL_Window *win = SDL_WINDOW(win_if);
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
    .resize = sdl_window_resize,
    .reset = sdl_window_reset,
    .update = sdl_window_update,
    .getWidth = sdl_window_getWidth,
    .getHeight = sdl_window_getHeight,
    .free = sdl_window_free,
    .pollEvent = PollEvent,
    .setPixel = sdl_window_set_pixel,
    .getPixel = sdl_window_get_pixel,
};
