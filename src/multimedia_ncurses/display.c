#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ncurses.h"
#include "coord.h"
#include "color.h"

#define RANGENC 1000
#define DOWNER 64
#define UPPER 192

static struct {
    Color untextured;
    int background;
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

// 0 <= COLOR <= 1000 et 0 <= r, g et b < range1
static void getCOLORFromRGB(int *r, int *g, int *b)
{
    *r = *r * RANGENC / (display.range1 - 1);
    *g = *g * RANGENC / (display.range1 - 1);
    *b = *b * RANGENC / (display.range1 - 1);
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

static void getRGBFromColor(const Color *c, int *r, int *g, int *b)
{
    *r = c->r * (display.range1 - 1) / 255;
    *g = c->g * (display.range1 - 1) / 255;
    *b = c->b * (display.range1 - 1) / 255;
}

int getIdFromRGB(int r, int g, int b)
{
    int id = r * display.range2 + g * display.range1 + b;
    if (id == 0)
	return display.background;
    else if (id == display.background)
	return 0;
    else
	return id;
}

// 0 <= id < nbColor et 0 <= r, g et b < range1
static int getIdFromColor(const Color *c)
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

static void getIntervalId(const Color *c, int *downer, int *upper)
{
    int r, g, b;
    getRGBFromColor(c, &r, &g, &b);
    int R = c->r * (display.range1 - 1) - r * 255;
    int G = c->g * (display.range1 - 1) - g * 255;
    int B = c->b * (display.range1 - 1) - b * 255;
    int downerR, upperR, downerG, upperG, downerB, upperB;
    getIntervalComponent(R, r, &downerR, &upperR);
    getIntervalComponent(G, g, &downerG, &upperG);
    getIntervalComponent(B, b, &downerB, &upperB);
    *downer = getIdFromRGB(downerR, downerG, downerB);
    *upper = getIdFromRGB(upperR, upperG, upperB);    
}

static int initColor(const Color *background, const Color *untextured)
{
    if (can_change_color() == FALSE)
	return 0;
    int r, g, b;
    start_color();
    display.untextured = *untextured;
    display.range1 = getRange(min(COLOR_PAIRS, COLORS));
    display.range2 = display.range1 * display.range1;
    display.nbColor = display.range2 * display.range1;

    for (int i = 1; i < display.nbColor; i++) {
	getCOLORFromId(i, &r, &g, &b);
	setIdWithCOLOR(i, r, g, b);
    }
    display.background = getIdFromColor(background);
    getCOLORFromId(display.background, &r, &g, &b);
    setIdWithCOLOR(display.background, 0, 0, 0);
    setIdWithCOLOR(0, r, g, b);
    return 1;
}

void initDisplay_(int screenWidth, int screenHeight, const Color *background, 
		  const Color *untextured)
{
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    nodelay(stdscr, TRUE);
    cbreak();
    curs_set(0);
    if(!initColor(background, untextured)) {
	endwin();
	printf("Unable to change colors. Try <export TERM=xterm-256color\n");
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
	int downer, upper;
	getIntervalId(color, &downer, &upper);
	attron(COLOR_PAIR(downer));
	mvprintw(A->h, 2 * A->w, " ");
	attroff(COLOR_PAIR(downer));
	attron(COLOR_PAIR(upper));
	mvprintw(A->h, 2 * A->w + 1, " ");
	attroff(COLOR_PAIR(upper));
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
