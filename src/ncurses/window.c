#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ncurses.h>

#include "coord.h"
#include "color.h"
#include "window.h"

#define RANGENC 1000
#define DOWNER 64
#define UPPER 192

bool ncurses_window_pollEvent(Window *w, Event *event);
void ncurses_window_waitEvent(Window *w, Event *event);

typedef struct ncurses_Window ncurses_Window;
struct ncurses_Window {
    Window parent;
    int background;
    short nbColor;
    short range1;
    short range2;
};

static Window _win_ops;
static ncurses_Window *window;

static inline int powi(int a, int b)
{
    int p = 1;
    for (int i = 0; i < b; i++)
        p *= a;
    return p;
}

static int getRange(int nbColor)
{
    int a = 0;
    int b = nbColor;
    while (b - a > 1) {
        int c = (a + b) / 2;
        if (powi(c, 3) > nbColor)
            b = c;
        else
            a = c;
    }
    return a;
}

// 0 <= COLOR <= 1000 et 0 <= r, g et b < range1
static void getCOLORFromRGB(int *r, int *g, int *b)
{
    *r = *r * RANGENC / (window->range1 - 1);
    *g = *g * RANGENC / (window->range1 - 1);
    *b = *b * RANGENC / (window->range1 - 1);
}

// 0 <= id < nbColor et 0 <= r, g et b < range1
static void getRGBFromId(int id, int *r, int *g, int *b)
{
    *r = id / window->range2;
    id -= *r * window->range2;
    *g = id / window->range1;
    id -= *g * window->range1;
    *b = id;
}

static void getRGBFromColor(Color c, int *r, int *g, int *b)
{
    *r = c.r * (window->range1 - 1) / 255;
    *g = c.g * (window->range1 - 1) / 255;
    *b = c.b * (window->range1 - 1) / 255;
}

int getIdFromRGB(int r, int g, int b)
{
    int id = r * window->range2 + g * window->range1 + b;
    if (id == 0)
        return window->background;
    else if (id == window->background)
        return 0;
    else
        return id;
}

// 0 <= id < nbColor et 0 <= r, g et b < range1
static int getIdFromColor(Color c)
{
    int r, g, b;
    getRGBFromColor(c, &r, &g, &b);
    return getIdFromRGB(r, g, b);
}

static void getCOLORFromId(int id, int *r, int *g, int *b)
{
    getRGBFromId(id, r, g, b);
    getCOLORFromRGB(r, g, b);
}

static void setIdWithCOLOR(int id, int r, int g, int b)
{
    init_color(id, r, g, b);
    init_pair(id, id, id);
}

void getIntervalComponent(int coord, int component, int *downer, int * upper)
{
    if (coord < DOWNER) {
        *downer = component;
        *upper = component;
    } else if (coord >= UPPER) {
        *downer = component + 1;
        *upper = component + 1;
    } else {
        *downer = component;
        *upper = component + 1;
    }
}

static void getIntervalId(Color c, int *downer, int *upper)
{
    int r, g, b;
    getRGBFromColor(c, &r, &g, &b);
    int R = c.r * (window->range1 - 1) - r * 255;
    int G = c.g * (window->range1 - 1) - g * 255;
    int B = c.b * (window->range1 - 1) - b * 255;
    int downerR, upperR, downerG, upperG, downerB, upperB;
    getIntervalComponent(R, r, &downerR, &upperR);
    getIntervalComponent(G, g, &downerG, &upperG);
    getIntervalComponent(B, b, &downerB, &upperB);
    *downer = getIdFromRGB(downerR, downerG, downerB);
    *upper = getIdFromRGB(upperR, upperG, upperB);
}

static int initColor(Color background)
{
    if (can_change_color() == FALSE)
        return 0;
    int r, g, b;
    start_color();
    window->range1 = getRange(MIN(COLOR_PAIRS, COLORS));
    window->range2 = window->range1 * window->range1;
    window->nbColor = window->range2 * window->range1;

    for (int i = 1; i < window->nbColor; i++) {
        getCOLORFromId(i, &r, &g, &b);
        setIdWithCOLOR(i, r, g, b);
    }
    window->background = getIdFromColor(background);
    getCOLORFromId(window->background, &r, &g, &b);
    setIdWithCOLOR(window->background, 0, 0, 0);
    setIdWithCOLOR(0, r, g, b);
    return 1;
}

Window* CreateWindow(int width, int height, Color background)
{
    (void) width;
    (void) height;

    initscr();
    keypad(stdscr, TRUE);
    noecho();
    nodelay(stdscr, TRUE);
    cbreak();
    curs_set(0);
    if (window != NULL) {
        fprintf(stderr, "Only one ncurses window is available\n");
        return (Window*) window;
    }

    window = malloc(sizeof*window);
    if (!initColor(background)) {
        endwin();
        printf("Unable to change colors. Try <export TERM=xterm-256color\n");
        exit(EXIT_FAILURE);
    }
    window->parent = _win_ops;

    return (Window*) window;
}

static void ncurses_window_reset(Window *w)
{
    clear();
    (void) w; // TODO: implement background color filling
}

static Color
ncurses_window_getPixel(Window *w, Coord A)
{
    (void) w;
    Color c = COLOR(0,0,0);
    return c;
}

static void
ncurses_window_setPixel(Window *w, Coord A, Color color)
{
    (void) w;
    int maxW = 0;
    int maxH = 0;

    getmaxyx(stdscr, maxH, maxW);
    if (A.w >= 0 && A.w < maxW / 2 && A.h >= 0 && A.h < maxH) {
        int downer, upper;
        getIntervalId(color, &downer, &upper);
        attron(COLOR_PAIR(downer));
        mvprintw(A.h, 2 * A.w, " ");
        attroff(COLOR_PAIR(downer));
        attron(COLOR_PAIR(upper));
        mvprintw(A.h, 2 * A.w + 1, " ");
        attroff(COLOR_PAIR(upper));
    }
}

static void
ncurses_window_flip(Window *w)
{
    (void) w;
    refresh();
}

static int
ncurses_window_getWidth(Window *w)
{
    (void) w;
    return stdscr->_maxx;
}

static int
ncurses_window_getHeight(Window *w)
{
    (void) w;
    return stdscr->_maxy;
}

static void
ncurses_window_free(Window *w)
{
    (void) w;
    endwin();
}

static Window _win_ops = {
    .reset = ncurses_window_reset,
    .update = ncurses_window_flip,
    .getWidth = ncurses_window_getWidth,
    .getHeight = ncurses_window_getHeight,
    .getPixel = ncurses_window_getPixel,
    .setPixel = ncurses_window_setPixel,
    .free = ncurses_window_free,
    .pollEvent = ncurses_window_pollEvent,
    .waitEvent = ncurses_window_waitEvent,
};
