#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "scene.h"
#include "event.h"
#include "point.h"
#include "frame.h"
#include "solid.h"
#include "color.h"
#include "camera.h"
#include "SDL/SDL.h"

#define MAXLENGTH 128

static struct {
    Frame origin;

    Point light;

    Solid **solidBuffer;
    int nbSolid;
    int bufferSize;

    SDL_Surface *screen;
    Camera *c;
} scene;

static void initLight(Point *light, float x, float y, float z)
{
    setPoint(light, x, y, z);
    normalizePoint(light, light);
}

static void initSDL(int screenWidth, int screenHeight)
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
	fprintf(stderr, "Error SDL_Init: %s", SDL_GetError());
	exit(EXIT_FAILURE);
    }
    if ((scene.screen = SDL_SetVideoMode(screenWidth, screenHeight, 
					 32, SDL_HWSURFACE | SDL_DOUBLEBUF)) 
	== NULL) {
	fprintf(stderr, "Error SDL_SetVideoMode: %s", SDL_GetError());
	exit(EXIT_FAILURE);
    }
    SDL_WM_SetCaption("3Displayer", NULL);
    SDL_EnableKeyRepeat(1, 10);
}

void initScene(void)
{
    int screenWidth = 1200;
    int screenHeight = 800;

    
    scene.c = initCamera();
    Frame frame1;
    Frame frame2;
    Coord coord1;
    Coord coord2;
    resetFrame(&frame1, -0.5, -5., 0);
    resetFrame(&frame2, 0.5, -5., 0);
    setCoord(&coord1, 10, 10);
    setCoord(&coord2, screenWidth / 2 + 10, 10);
    addLensToCamera(scene.c, &frame2, &coord2, screenWidth / 2 - 20, screenHeight - 20, 
		    1., 20., 80, 60);
    
    addLensToCamera(scene.c, &frame1, &coord1, screenWidth / 2 - 20, screenHeight - 20, 
		    1., 20., 80, 60);
    
    resetCamera(scene.c);

    initLight(&scene.light, 1., -0.5, -2.);

    resetFrame(&scene.origin, 0., 0., 0.);
    resetEvent();
    
    scene.nbSolid = 0;
    scene.bufferSize = 4;
    scene.solidBuffer = malloc(scene.bufferSize * sizeof(Solid*));

    initSDL(screenWidth, screenHeight);
}

void updateScene(int *stop)
{
    SDL_Event event;
    SDL_WaitEvent(&event);
    do {
	switch (event.type) {
	case SDL_QUIT:
	    *stop = 1;
	    break;
	case SDL_KEYDOWN:
	    handleKeyDownEvent(&event);
	    break;
	case SDL_KEYUP:
	    handleKeyUpEvent(&event);
	    break;
	case SDL_MOUSEMOTION:
	    handleMouseMotionEvent(&event);
	    break;
	case SDL_MOUSEBUTTONUP:
	    handleMouseButtonUpEvent(&event);
	    break;
	case SDL_MOUSEBUTTONDOWN:
	    handleMouseButtonDownEvent(&event);
	    break;
	}
    } while (SDL_PollEvent(&event));   
}

static void remove_space(char *buf)
{
    while (*buf) {
	if (*buf == ' ') {
	    *buf = 0;
	    return;
	}
	++buf;
    }
}

void askSolidForScene(void)
{
    Solid *solid;
    char *buf, *objstr;
    
    buf = readline("obj path: ");
    remove_space(buf);
    objstr = strdup(buf);
    add_history(buf);
    buf = readline("bmp path: ");
    remove_space(buf);    

    if ((solid = loadSolid(objstr, buf)))
	addSolidToScene(solid);
    free(objstr);
    add_history(buf);
}

void askEquationForScene(void)
{
    Solid *solid;
    char *buf, *eqstr;
    
    buf = readline("eq path: ");
    remove_space(buf);
    eqstr = strdup(buf);
    add_history(buf);
    buf = readline("bmp path: ");
    remove_space(buf);
    
    if ((solid = equationSolid(eqstr, buf)))
	addSolidToScene(solid);
    free(eqstr);
    add_history(buf);
}

void addSolidToScene(Solid *solid)
{
    if(scene.nbSolid >= scene.bufferSize){
	scene.bufferSize *= 2;
	scene.solidBuffer = realloc(scene.solidBuffer, 
				    scene.bufferSize * 
				    sizeof(Solid *));
    }
    scene.solidBuffer[scene.nbSolid++] = solid;   
}

void removeSolidFromScene()
{
    if(scene.nbSolid > 0)
	freeSolid(scene.solidBuffer[--scene.nbSolid]);
}
    
void drawScene(void)
{
    Camera *C = scene.c;
    Color color;

    resetCamera(C);
    SDL_FillRect(scene.screen, NULL, SDL_MapRGB(scene.screen->format, 
						128, 128, 128));

    for (initIteratorCamera(C); condIteratorCamera(C); nextIteratorCamera(C)) {
	if (getDrawEvent())
	    for (int i = 0; i < scene.nbSolid; i++)
		drawSolid(lensIteratorCamera(C), scene.solidBuffer[i]);
	if (getWireframeEvent())
	    for (int i = 0; i < scene.nbSolid; i++)
		wireframeSolid(lensIteratorCamera(C), scene.solidBuffer[i], 
			       setColor(&color, 255, 0, 0));
	if (getNormalEvent())
	    for (int i = 0; i < scene.nbSolid; i++)
		normalSolid(lensIteratorCamera(C), scene.solidBuffer[i], 
			    setColor(&color, 0, 255, 0));
	if (getVertexEvent())
	    for (int i = 0; i < scene.nbSolid; i++)
		vertexSolid(lensIteratorCamera(C), scene.solidBuffer[i], 
			    setColor(&color, 0, 0, 255));
	if (getFrameEvent())
	    drawFrame(lensIteratorCamera(C), &scene.origin);
    }
    SDL_Flip(scene.screen);
}

void handleArgumentScene(int argc, char *argv[])
{
    Solid *solid;

    switch (argc) {
    case 3:
	if ((strcmp(argv[1], "-l") == 0 && 
	     (solid = loadSolid(argv[2], NULL))) ||
	    (strcmp(argv[1], "-e") == 0 && 
	     (solid = equationSolid(argv[2], NULL))))
	    addSolidToScene(solid);
	break;
    case 4:
	if ((strcmp(argv[1], "-l") == 0 && 
	     (solid = loadSolid(argv[2], argv[3]))) ||
	    (strcmp(argv[1], "-e") == 0 && 
	     (solid = equationSolid(argv[2], argv[3]))))
	    addSolidToScene(solid);
	break;
    default:
	break;
    }
}

Point *getLight(void)
{
    return &scene.light;
}

SDL_Surface *getScreen(void)
{
    return scene.screen;
}

Camera *getCamera(void)
{
    return scene.c;
}

void freeScene(void)
{
    for(int i = 0; i < scene.nbSolid; i++)
	freeSolid(scene.solidBuffer[i]);
    free(scene.solidBuffer);
    freeCamera(scene.c);
    SDL_FreeSurface(scene.screen);
    SDL_Quit();
}
