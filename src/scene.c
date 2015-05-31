#include <stdio.h>
#include <stdlib.h>

#include "scene.h"
#include "event.h"
#include "point.h"
#include "frame.h"
#include "solid.h"
#include "color.h"
#include "SDL/SDL.h"

#define MAXLENGTH 128

static struct {
    Frame camera;
    Frame origin;

    Point light;

    Solid **solidBuffer;
    int nbSolid;
    int bufferSize;

    SDL_Surface *screen;
    int wfov;
    int hfov;
    int screenWidth;
    int screenHeight;
    float nearplan;
    float farplan;
    float *zBuffer;
} scene;

static void resetZBuffer(float *zBuffer)
{
    int size = scene.screenWidth * scene.screenHeight;
    for (int i = 0; i < size; i++)
	zBuffer[i] = -1.;
}

static void initLight(Point *light, float x, float y, float z)
{
    setPoint(light, x, y, z);
    normalizePoint(light, light);
}

static void initSDL(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
	fprintf(stderr, "Error SDL_Init: %s", SDL_GetError());
	exit(EXIT_FAILURE);
    }
    if ((scene.screen = SDL_SetVideoMode(scene.screenWidth, scene.screenHeight, 
					 32, SDL_HWSURFACE | SDL_DOUBLEBUF)) 
	== NULL) {
	fprintf(stderr, "Error SDL_SetVideoMode: %s", SDL_GetError());
	exit(EXIT_FAILURE);
    }
    SDL_WM_SetCaption("3Displayer", NULL);
    SDL_EnableKeyRepeat(1, 10);
}

static void cutNewLine(char *input)
{
    int i;
    for (i = 0; input[i] && input[i] != '\n'; i++);
    input[i] = '\0';
}

void initScene(void)
{
    scene.nbSolid = 0;
    scene.bufferSize = 4;
    scene.wfov = 80;
    scene.hfov = 60;
    scene.screenWidth = 1200;
    scene.screenHeight = 900;
    scene.nearplan = 1.;
    scene.farplan = 20.;
    
    initLight(&scene.light, 1., -0.5, -2.);
    resetFrame(&scene.camera, 0., -5., 0.);
    resetFrame(&scene.origin, 0., 0., 0.);
    resetEvent();

    scene.solidBuffer = malloc(scene.bufferSize * sizeof(Solid*));
    scene.zBuffer = malloc(scene.screenWidth * scene.screenHeight * 
			   sizeof(float));
    initSDL();
}

void updateScene(int *stop)
{
    SDL_Event event;
    SDL_WaitEvent(&event);

    switch (event.type) {
    case SDL_QUIT:
	*stop = 1;
	break;
    case SDL_KEYDOWN:
	handleKeyDownEvent(&event, stop);
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
}

void askSolidForScene(void)
{
    Solid *solid;
    char objName[MAXLENGTH];
    char bmpName[MAXLENGTH];

    printf(".obj path: ");
    fgets(objName, MAXLENGTH, stdin);
    cutNewLine(objName);

    printf(".bmp path: ");
    fgets(bmpName, MAXLENGTH, stdin);
    cutNewLine(bmpName);

    if ((solid = loadSolid(objName, bmpName)))
	addSolidToScene(solid);
}

void askEquationForScene(void)
{
    Solid *solid;
    char eqName[MAXLENGTH];
    char bmpName[MAXLENGTH];

    printf(".eq path: ");
    fgets(eqName, MAXLENGTH, stdin);
    cutNewLine(eqName);
    
    printf(".bmp path: ");
    fgets(bmpName, MAXLENGTH, stdin);
    cutNewLine(bmpName);

    if ((solid = equationSolid(eqName, bmpName)))
	addSolidToScene(solid);
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
    SDL_FillRect(scene.screen, NULL, SDL_MapRGB(scene.screen->format, 
						128, 128, 128));
    resetZBuffer(scene.zBuffer);
    Color color;
    if (getDrawEvent())
	for (int i = 0; i < scene.nbSolid; i++)
	    drawSolid(scene.solidBuffer[i]);
    if (getWireframeEvent())
	for (int i = 0; i < scene.nbSolid; i++)
	    wireframeSolid(scene.solidBuffer[i], setColor(&color, 255, 0, 0));
    if (getNormalEvent())
	for (int i = 0; i < scene.nbSolid; i++)
	    normalSolid(scene.solidBuffer[i], setColor(&color, 0, 255, 0));
    if (getVertexEvent())
	for (int i = 0; i < scene.nbSolid; i++)
	    vertexSolid(scene.solidBuffer[i], setColor(&color, 0, 0, 255));
    if (getFrameEvent())
	drawFrame(&scene.origin);
    SDL_Flip(scene.screen);
}

void handleArgumentScene(int argc, char *argv[])
{
    Solid *solid;
    switch (argc) {
    case 1:
	break;
    case 2:
	if ((solid = loadSolid(argv[1], NULL)))
	    addSolidToScene(solid);
	break;
    default:
	if ((solid = loadSolid(argv[1], argv[2])))
	    addSolidToScene(solid);
	break;
    }
}


Frame *getCamera(void)
{
    return &scene.camera;
}

Point *getLight(void)
{
    return &scene.light;
}

SDL_Surface *getScreen(void)
{
    return scene.screen;
}

int getHfov(void)
{
    return scene.hfov;
}

int getWfov(void)
{
    return scene.wfov;
}

int getScreenWidth(void)
{
    return scene.screenWidth;
}

int getScreenHeight(void)
{
    return scene.screenHeight;
}

float getNearplan(void)
{
    return scene.nearplan;
}

float getFarplan(void)
{
    return scene.farplan;
}

float *getZBuffer(void)
{
    return scene.zBuffer;
}

void freeScene(void)
{
    for(int i = 0; i < scene.nbSolid; i++)
	freeSolid(scene.solidBuffer[i]);
    free(scene.solidBuffer);
    free(scene.zBuffer);
    SDL_FreeSurface(scene.screen);
    SDL_Quit();
}
