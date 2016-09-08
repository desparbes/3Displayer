#include <ncurses.h>
#include "scene.h"
#include "window.h"


bool PollEvent(Event *event)
{
    int c;
    if ((c = getch()) == ERR)
        return false;

    switch (c) {
    case KEY_LEFT:
        event->type = ROTATE;
        event->direction = LEFT;
        break;
    case KEY_RIGHT:
        event->type = ROTATE;
        event->direction = RIGHT;
        break;
    case KEY_UP:
        event->type = ROTATE;
        event->direction = UP;
        break;
    case KEY_DOWN:
        event->type = ROTATE;
        event->direction = DOWN;
        break;
    case KEY_RESIZE:
    case 'r':{
        event->type = RESIZE;
        getmaxyx(stdscr, event->height, event->width);
        event->width /= 2;
        break;
    }
    case 'q':
        event->type = TRANSLATE;
        event->direction = LEFT;
        break;
    case 'z':
        event->type = TRANSLATE;
        event->direction = FORWARD;
        break;
    case 'd':
        event->type = TRANSLATE;
        event->direction = RIGHT;
        break;
    case 's':
        event->type = TRANSLATE;
        event->direction = BACKWARD;
        break;
    case 'a':
        event->type = TRANSLATE;
        event->direction = DOWN;
        break;
    case 'e':
        event->type = TRANSLATE;
        event->direction = UP;
        break;
    case 'p':
        event->type = QUIT;
        break;
    case 'o':
        event->type = STATE;
        event->state = DRAW;
        break;
    case 'w':
        event->type = STATE;
        event->state = WIREFRAME;
        break;
    case 'n':
        event->type = STATE;
        event->state = NORMAL;
        break;
    case 'v':
        event->type = STATE;
        event->state = VERTEX;
        break;
    case 'f':
        event->type = STATE;
        event->state = FRAME;
        break;
    case 'l':
        event->type = LOAD;
        break;
    case 'u':
        event->type = UNLOAD;
        break;
    default:
        return false;
        break;
    }
    return true;
}

