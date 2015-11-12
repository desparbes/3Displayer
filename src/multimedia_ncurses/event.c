#include "ncurses.h"
#include "scene.h"
#include "direction.h"
#include "view.h"

void handleEvent_(int *stop)
{
    int c = getch();
    switch (c) {
    case KEY_LEFT:
	rotateCameraScene(LEFT);
	break;
    case KEY_RIGHT:
	rotateCameraScene(RIGHT);	
	break;
    case KEY_UP:
	rotateCameraScene(UP);
	break;
    case KEY_DOWN:
	rotateCameraScene(DOWN);	
	break;
    case KEY_RESIZE: {
	int screenWidth, screenHeight;
	getmaxyx(stdscr, screenHeight, screenWidth);
	resizeCameraScene(screenWidth / 2, screenHeight);
	break;
    }
    case 'q':
	translateCameraScene(LEFT);
	break;
    case 'z':
	translateCameraScene(FORWARD);
	break;
    case 'd':
	translateCameraScene(RIGHT);
	break;
    case 's':
	translateCameraScene(BACKWARD);
	break;
    case 'a':
	translateCameraScene(DOWN);
	break;
    case 'e':
	translateCameraScene(UP);
	break;
    case 'p':
	*stop = 1;
	break;
    case 'o':
	switchStateCameraScene(DRAW);
	break;
    case 'w':
	switchStateCameraScene(WIREFRAME);
	break;
    case 'n':
	switchStateCameraScene(NORMAL);
	break;
    case 'v':
	switchStateCameraScene(VERTEX);
	break;
   case 'f':
	switchStateCameraScene(FRAME);
	break;
    case 'l':
	clear();
	refresh();
	askSolidForScene();
	break;
    case 'u':
	removeSolidFromScene();
	break;
    default:
	break;
    }
}

void (*handleEvent)(int *) = &handleEvent_;
