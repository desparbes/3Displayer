#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ncurses.h"
#include "coord.h"
#include "color.h"

#define RANGENC 1000

static struct {
    Color untextured;
    short nbColor;
    short range1;
    short range2;
} display;

static inline int min(int a, int b)
{
    return a < b ? a : b;
}

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

// 0 <= id < nbColor et 0 <= r, g et b < range1
static void getRGBFromId(int id, int *r, int *g, int *b)
{
    *r = id / display.range2;
    id -= *r * display.range2;
    *g = id / display.range1;
    id -= *g * display.range1;
    *b = id;
}

// 0 <= id < nbColor et 0 <= r, g et b < range1
static int getIdFromColor(const Color *c)
{
    int r, g, b;
    r = c->r * (display.range1 - 1) / 255;
    g = c->g * (display.range1 - 1) / 255;
    b = c->b * (display.range1 - 1) / 255;
    return r * display.range2 + g * display.range1 + b;
}

// 0 <= COLOR <= 1000 et 0 <= r, g et b < range1
static void getCOLORFromRGB(int *r, int *g, int *b)
{
    *r = *r * RANGENC / (display.range1 - 1);
    *g = *g * RANGENC / (display.range1 - 1);
    *b = *b * RANGENC / (display.range1 - 1);
}

static int initColor()
{
    if (has_colors() == FALSE || can_change_color() == FALSE)
	return 0;

    start_color();
    display.range1 = getRange(min(COLOR_PAIRS, COLORS));
    display.range2 = display.range1 * display.range1;
    display.nbColor = display.range2 * display.range1;
    for (int i = 0; i < display.nbColor; i++) {
	int r, g, b;
	getRGBFromId(i, &r, &g, &b);
	getCOLORFromRGB(&r, &g, &b);
	init_color(i, r, g, b);
	init_pair(i, i, i);
    }
    return 1;
}

void initDisplay_(int screenWidth, int screenHeight, const Color *background, 
		  const Color *untextured)
{
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    curs_set(0);
    display.untextured = *untextured;
    if(!initColor()) {
	mvprintw(0, 0, "Terminal does not support colors\n");
	getch();
	endwin();
	exit(1);
    }	
}

void resizeDisplay_(int screenWidth, int screenHeight)
{
    resize_term(screenHeight, 2 * screenWidth);
}

void resetDisplay_() 
{
    clear();
}

void getPixelDisplay_(const Coord *A, Color *color) {}

void setPixelDisplay_(const Coord *A, const Color *color)
{
    int maxW = 0;
    int maxH = 0;
    getmaxyx(stdscr, maxH, maxW);
    if (A->w >= 0 && A->w < maxW / 2 && A->h >= 0 && A->h < maxH) {
	int c = getIdFromColor(color);
	int r, g, b;
	getRGBFromId(c, &r, &g, &b);
	getCOLORFromRGB(&r, &g, &b);
	attron(COLOR_PAIR(c));
	mvprintw(A->h, 2 * A->w, "  ");
	attroff(COLOR_PAIR(c));
    }
}

void blitDisplay_()
{
    refresh();
}

void getUntexturedDisplay_(Color *c)
{
    *c = display.untextured;
}

int getWidthDisplay_()
{
    return stdscr->_maxx;
}

int getHeightDisplay_()
{
    return stdscr->_maxy;
}

void freeDisplay_()
{
    endwin();
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
