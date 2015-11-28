#include <stdio.h>
#include <stdlib.h>

#include "ncurses.h"
#include "coord.h"
#include "color.h"

#define RANGENC 1000

static struct {
    int range1;
    int range2;
    short *colorBuffer;
    Color untextured;
    int sup;
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
	if (powi(c, 3) >= nbColor)
	    b = c;
	else
	    a = c;
    }
    return b;
}

// 0 <= COLOR <= 1000 et 0 <= r, g et b < range1
static void getCOLORFromRGB(int R, int G, int B, 
			    short *r, short *g, short *b)
{
    *r = R * RANGENC / (display.range1 - 1);
    *g = G * RANGENC / (display.range1 - 1);
    *b = B * RANGENC / (display.range1 - 1);
}

// 0 <= id < nbColor et 0 <= r, g et b < range1
static void getRGBFromId(int id, int *R, int *G, int *B)
{
    *R = id / display.range2;
    id -= *R * display.range2;
    *G = id / display.range1;
    id -= *G * display.range1;
    *B = id;
}

// 0 <= id < nbColor et 0 <= r, g et b < range1
static int getIdFromColor(const Color *c)
{
    int R, G, B;
    R = c->r * (display.range1 - 1) / 255;
    G = c->g * (display.range1 - 1) / 255;
    B = c->b * (display.range1 - 1) / 255;
    return R * display.range2 + G * display.range1 + B;
}

static void getCOLORFromId(int id, short *r, short *g, short *b)
{
    int R, G, B;
    getRGBFromId(id, &R, &G, &B);
    getCOLORFromRGB(R, G, B, r, g, b);
}

static void getCOLORFromColor(const Color *c, short *r, short *g, short *b)
{
    *r = c->r * RANGENC / 255;
    *g = c->g * RANGENC / 255;
    *b = c->b * RANGENC / 255;
}
/*
static short distanceCOLOR(short rA, short gA, short bA, 
			   short rB, short gB, short bB)
{
    return abs(rB - rA) + abs(gB - gA) + abs(bB - bA);
}

static short getPositionOfNearestCOLOR(short r, short g, short b)
{
    short minId = -1;
    short minDist = 3000;

    //endwin();
    //printf("COLORS: %d\n", COLORS);

    for (short i = 0; i < COLORS; i++) {
        short ri, gi, bi;
	color_content(i, &ri, &gi, &bi); //BUG pour xterm-256color
        short tmp = distanceCOLOR(r, g, b, ri, gi, bi);
	//printf("(%d %d %d) et (%d, %d, %d): %d\n", r, g, b, ri, gi, bi, tmp);
        if (tmp < minDist) {
            minDist = tmp;
            minId = i;
        }
    }
    //printf("\n");
    return minId;
}
*/
static short getPositionFromColor(const Color *color)
{
    return display.colorBuffer[getIdFromColor(color)];	
}

static void setPositionWithCOLOR(short p, short r, short g, short b)
{
    init_color(p, r, g, b);
    init_pair(p, p, p);
}

// COLORS doit etre non nul
static void initColorBuffer()
{
/*
    endwin();
*/    
    int sup = display.range2 * display.range1;
    display.colorBuffer = malloc(sup * sizeof(short));
    for (int i = 1, j = 1, I = COLORS, J = sup; i < sup; i++, I += COLORS) {
	if (J > I - COLORS && J <= I) {
/*
	    short r, g, b;
	    getCOLORFromId(i, &r, &g, &b);
	    int p = getPositionOfNearestCOLOR(r, g, b);
*/
	    short r, g, b;
	    getCOLORFromId(i, &r, &g, &b);
	    setPositionWithCOLOR(j, r, g, b);
	    display.colorBuffer[i] = j;
	    J += sup;
	    j++;
	} else {
	    display.colorBuffer[i] = display.colorBuffer[i - 1];
	}
    }
    display.sup = sup;

    endwin();
    for (int i = 0; i < sup; i++) {
	short r, g, b;
	color_content(display.colorBuffer[i], &r, &g, &b);
	mvprintw(i, 0, "id: %d, position: %d, COLOR: (%d, %d, %d)\n", i, display.colorBuffer[i], r, g, b);
    }
    refresh();
    getch();
}

static int initColor(const Color *background, const Color *untextured)
{
    //if (can_change_color() == FALSE)
//	return 0;
    short r, g, b;
    start_color();
    display.range1 = getRange(COLORS);
    display.range2 = display.range1 * display.range1;
    display.untextured = *untextured;
    initColorBuffer();
    int backId = getIdFromColor(background);
/*
    endwin();
    printf("backId: %d\n", backId);
*/
    getCOLORFromColor(background, &r, &g, &b);
    setPositionWithCOLOR(0, r, g, b);
//  bkgd(COLOR_PAIR(display.colorBuffer[backId]));
    setPositionWithCOLOR(display.colorBuffer[backId], 0, 0, 0);

    display.colorBuffer[0] = display.colorBuffer[backId];
    display.colorBuffer[backId] = 0;
    
    
    //swap noir et la couleur du background dans le colorBuffer
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
	printf("Terminal does not support colors. Try <export TERM=xterm-256color>\n");
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
	//int id = getIdFromColor(color);
	short c = getPositionFromColor(color);//id * COLORS / display.sup;
	attron(COLOR_PAIR(c));
	mvprintw(A->h, 2 * A->w, "  ");
	attroff(COLOR_PAIR(c));
/*
	endwin();
	short r, g, b;
	color_content(c, &r, &g, &b);
	printf("id: %d, position: %d, COLOR: (%d %d %d), Color: (%d %d %d)\n", getIdFromColor(color), c, r, g, b, color->r, color->g, color->b);
*/

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
    free(display.colorBuffer);
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
