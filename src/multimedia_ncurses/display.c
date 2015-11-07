#include <stdio.h>
#include <stdlib.h>

#include "ncurses.h"
#include "coord.h"
#include "color.h"

static struct {
    Color untextured;
} display;

void initDisplay_(int screenWidth, int screenHeight, const Color *background, 
		  const Color *untextured)
{
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    if (has_colors() == TRUE) {	
	start_color();
    }
    curs_set(0);
    init_pair(0, COLOR_BLACK, COLOR_BLACK);
    init_pair(1, COLOR_BLUE,COLOR_BLUE);
    init_pair(2, COLOR_GREEN, COLOR_GREEN);
    init_pair(3, COLOR_CYAN, COLOR_CYAN);
    init_pair(4, COLOR_RED, COLOR_RED); 
    init_pair(5, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(6, COLOR_YELLOW,COLOR_YELLOW);
    init_pair(7, COLOR_WHITE, COLOR_WHITE);
    display.untextured = *untextured;
}

void resizeDisplay_(int screenWidth, int screenHeight)
{
    resize_term(screenHeight, 2 * screenWidth);
}

void resetDisplay_() 
{
    clear();
}

static int cutChar(unsigned char c)
{
    return c < 128 ? 0 : 1;
}

static int getColor(const Color *color)
{
    int c = 0;
    c = c | cutChar(color->b) << 0;
    c = c | cutChar(color->g) << 1;
    c = c | cutChar(color->r) << 2;
    return c;
}

void getPixelDisplay_(const Coord *A, Color *color)
{
    //chtype c = mvinch(A->h, A->w * 2);
}

void setPixelDisplay_(const Coord *A, const Color *color)
{
    int maxW = 0;
    int maxH = 0;
    getmaxyx(stdscr, maxH, maxW);
    if (A->w >= 0 && A->w < maxW / 2 && A->h >= 0 && A->h < maxH) {
	int c = getColor(color);
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
