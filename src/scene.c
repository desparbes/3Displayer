#include <stdio.h>
#include <stdlib.h>

#include "scene.h"
#include "point.h"
#include "frame.h"
#include "solid.h"
#include "color.h"
#include "SDL/SDL.h"

static struct {
    Frame camera;
    Frame origin;

    float translationSpeed;
    float rotationSpeed;
    
    int draw; // boolean : 1 draw the scene, else don't
    int wireframe; // boolean : 1 wireframe the scene, else don't
    int normal; // boolean : 1 display the normals of scene, else don't
    int vertex; // boolean : 1 display the vertex of the scene, else don't
    int frame; // boolean : 1 display the frame of the scene, else dont't

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

static void initSDL()
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

static void handleMouseEvent(SDL_Event *event)
{
    static float theta = 0., phi = 0., rho = 0.;
    static int mouseWidth = 0, mouseHeight = 0;
    static int rightClickDown = 0;
    switch (event->type) {
    case SDL_MOUSEMOTION:
	if (rightClickDown) {
	    if (mouseWidth > event->motion.x)
		theta -= scene.rotationSpeed;
	    else if (mouseWidth < event->motion.x)
		theta += scene.rotationSpeed;
	    else if (mouseHeight > event->motion.y)
		phi += scene.rotationSpeed;
	    else if (mouseHeight < event->motion.y)
		phi -= scene.rotationSpeed;
	}
	mouseWidth = event->motion.x;
	mouseHeight = event->motion.y;
	break;
    case SDL_MOUSEBUTTONUP:
	switch (event->button.button) {
	case SDL_BUTTON_RIGHT:
	    rightClickDown = 0;
	    break;
	}
	break;
    case SDL_MOUSEBUTTONDOWN:
	switch (event->button.button) {
	case SDL_BUTTON_RIGHT:
	    rightClickDown = 1;
	    break;
	}
	break;
    }
    printf("%d\n", rightClickDown);
    setDirectionFrame(&scene.camera, theta, phi, rho);
}

static void handleKeyboardEvent(SDL_Event *event, int *stop)
{
    switch (event->key.keysym.sym) {
    case SDLK_LEFT:
	translateFrame(&scene.camera, 
		       -scene.translationSpeed * scene.camera.i.x,
		       -scene.translationSpeed * scene.camera.i.y,
		       -scene.translationSpeed * scene.camera.i.z);
	break;
    case SDLK_RIGHT:
	translateFrame(&scene.camera, 
		       scene.translationSpeed * scene.camera.i.x,
		       scene.translationSpeed * scene.camera.i.y, 
		       scene.translationSpeed * scene.camera.i.z);
	break;
    case SDLK_UP:
	translateFrame(&scene.camera, 
		       scene.translationSpeed * scene.camera.j.x,
		       scene.translationSpeed * scene.camera.j.y, 
		       scene.translationSpeed * scene.camera.j.z);
	break;
    case SDLK_DOWN:
	translateFrame(&scene.camera, 
		       -scene.translationSpeed * scene.camera.j.x,
		       -scene.translationSpeed * scene.camera.j.y, 
		       -scene.translationSpeed * scene.camera.j.z);
	break;
    case SDLK_KP_PLUS:
	translateFrame(&scene.camera, 
		       scene.translationSpeed * scene.camera.k.x,
		       scene.translationSpeed * scene.camera.k.y, 
		       scene.translationSpeed * scene.camera.k.z);
	break;
    case SDLK_KP_MINUS:
	translateFrame(&scene.camera, 
		       -scene.translationSpeed * scene.camera.k.x,
		       -scene.translationSpeed * scene.camera.k.y, 
		       -scene.translationSpeed * scene.camera.k.z);
	break;
    case SDLK_ESCAPE:
	*stop = 1;
	break;
    case SDLK_r:
        resetFrame(&scene.camera);
	break;
    default:
	break;
    }
}

static void handleEvent(SDL_Event *event, int *stop)
{
    switch (event->type) {
    case SDL_QUIT:
	*stop = 1;
	break;
    case SDL_KEYDOWN:
	handleKeyboardEvent(event, stop);
	break;
    case SDL_MOUSEMOTION:
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
	handleMouseEvent(event);
	break;
    default:
	break;
    }
}

void initScene()
{
    scene.translationSpeed = 0.1;
    scene.rotationSpeed = 0.01;

    scene.draw = 1;
    scene.wireframe = 0;
    scene.normal = 0;
    scene.vertex = 0;
    scene.frame = 1;

    scene.nbSolid = 0;
    scene.bufferSize = 4;
    scene.wfov = 80;
    scene.hfov = 60;
    scene.screenWidth = 1200;
    scene.screenHeight = 900;
    scene.nearplan = 1.;
    scene.farplan = 20.;
    
    initLight(&scene.light, 1., -0.5, -2.);
    resetFrame(&scene.camera);
    resetFrame(&scene.origin);

    scene.solidBuffer = malloc(scene.bufferSize * sizeof(Solid*));
    scene.zBuffer = malloc(scene.screenWidth * scene.screenHeight * 
			   sizeof(float));
    initSDL();
}

void updateScene(int *stop)
{
    SDL_Event event;
    SDL_WaitEvent(&event);
    handleEvent(&event, stop);    
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

void removeSolidFromScene(Solid *solid)
{
    int i = 0;
    while (i < scene.nbSolid && scene.solidBuffer[i++] != solid); //Warning
    scene.solidBuffer[i] = scene.solidBuffer[--scene.nbSolid];
}
    
void drawScene()
{
    SDL_FillRect(scene.screen, NULL, SDL_MapRGB(scene.screen->format, 0, 0, 0));
    resetZBuffer(scene.zBuffer);
    Color color;
    if (scene.draw)
	for (int i = 0; i < scene.nbSolid; i++)
	    drawSolid(scene.solidBuffer[i]);
    if (scene.wireframe)
	for (int i = 0; i < scene.nbSolid; i++)
	    wireframeSolid(scene.solidBuffer[i], setColor(&color, 255, 0, 0));
    if (scene.normal)
	for (int i = 0; i < scene.nbSolid; i++)
	    normalSolid(scene.solidBuffer[i], setColor(&color, 0, 255, 0));
    if (scene.vertex)
	for (int i = 0; i < scene.nbSolid; i++)
	    vertexSolid(scene.solidBuffer[i], setColor(&color, 0, 0, 255));
    if (scene.frame)
	drawFrame(&scene.origin);
    SDL_Flip(scene.screen);
}

Frame *getCamera()
{
    return &scene.camera;
}

Point *getLight()
{
    return &scene.light;
}

SDL_Surface *getScreen()
{
    return scene.screen;
}

int getHfov()
{
    return scene.hfov;
}

int getWfov()
{
    return scene.wfov;
}

int getScreenWidth()
{
    return scene.screenWidth;
}

int getScreenHeight()
{
    return scene.screenHeight;
}

float getNearplan()
{
    return scene.nearplan;
}

float getFarplan()
{
    return scene.farplan;
}

float *getZBuffer()
{
    return scene.zBuffer;
}

void freeScene()
{
    for(int i = 0; i < scene.nbSolid; i++)
	freeSolid(scene.solidBuffer[i]);
    free(scene.solidBuffer);
    free(scene.zBuffer);
    SDL_FreeSurface(scene.screen);
    SDL_Quit();
}
