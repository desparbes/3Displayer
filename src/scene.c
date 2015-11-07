#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "scene.h"
#include "point.h"
#include "frame.h"
#include "solid.h"
#include "color.h"
#include "camera.h"
#include "display.h"
#include "view.h"
#include "multimedia.h"
#include "array.h"
#include "light.h"

#define MAXLENGTH 128
#define NB_KEYWORDS 6

enum {SCREENWIDTH, SCREENHEIGHT, BACKGROUND, UNTEXTURED, MULTIMEDIA, CAMERA};

static struct {
    Frame origin;

    Light **lightBuffer;
    int nbLight;
    int lightSize;

    Solid **solidBuffer;
    int nbSolid;
    int solidSize;

    Camera *camera;
} scene;


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

static void addSolidToScene(Solid *solid)
{
    if(scene.nbSolid >= scene.solidSize){
	scene.solidSize *= 2;
	scene.solidBuffer = realloc(scene.solidBuffer, 
				    scene.solidSize * 
				    sizeof(Solid *));
    }
    scene.solidBuffer[scene.nbSolid++] = solid;   
}

static void addLightToScene(Light *light)
{
    if(scene.nbLight >= scene.lightSize){
	scene.lightSize *= 2;
	scene.lightBuffer = realloc(scene.lightBuffer, 
				    scene.lightSize * 
				    sizeof(Light *));
    }
    scene.lightBuffer[scene.nbLight++] = light;   
}

static void freeSolidBuffer()
{
    for(int i = 0; i < scene.nbSolid; i++)
	freeSolid(scene.solidBuffer[i]);
    free(scene.solidBuffer);
}

static void freeLightBuffer()
{
    for(int i = 0; i < scene.nbLight; i++)
	freeLight(scene.lightBuffer[i]);
    free(scene.lightBuffer);
}

void initScene(void)
{
    Color background;
    Color untextured;
    int screenWidth, screenHeight;
    initFrame(&scene.origin);
    char *fileName = "config/config.txt";
    scene.camera = NULL;
    scene.nbSolid = 0;
    scene.solidSize = 4;
    scene.solidBuffer = malloc(scene.solidSize * sizeof(Solid*));
    scene.nbLight = 0;
    scene.lightSize = 4;
    scene.lightBuffer = malloc(scene.lightSize * sizeof(Light*));
    FILE *file = fopen(fileName, "r");
    char camera[MAXLENGTH];
    char multimedia[MAXLENGTH];
    int check[NB_KEYWORDS] = {0};
    int template[NB_KEYWORDS];
    initArray(template, NB_KEYWORDS, 1);

    if (file == NULL) {
	printf("File %s not found\n", fileName);
    } else {
	char str[MAXLENGTH];
	while (fscanf(file, "%s", str) != EOF) {
	    if (strcmp(str, "screenWidth") == 0 &&
		fscanf(file, "%d", &screenWidth) == 1)
		check[SCREENWIDTH]++;
	    else if (strcmp(str, "screenHeight") == 0 &&
		     fscanf(file, "%d", &screenHeight) == 1)
		check[SCREENHEIGHT]++;
	    else if (strcmp(str, "light") == 0 &&
		     fscanf(file, "%s", str) == 1)
		addLightToScene(loadLight(str));
	    else if (strcmp(str, "background") == 0 &&
		     fscanf(file, "%hhd %hhd %hhd", 
			    &background.r, 
			    &background.g, 
			    &background.b) == 3)
		check[BACKGROUND]++;
	    else if (strcmp(str, "untextured") == 0 &&
		     fscanf(file, "%hhd %hhd %hhd", 
			    &untextured.r, 
			    &untextured.g, 
			    &untextured.b) == 3)
		check[UNTEXTURED]++;
	    else if (strcmp(str, "multimedia") == 0 &&
		     fscanf(file, "%s", multimedia) == 1)
		check[MULTIMEDIA]++;
	    else if (strcmp(str, "camera") == 0 &&
		     fscanf(file, "%s", camera) == 1)
		check[CAMERA]++;
	}
	fclose(file);
    }

    if (!areEqualsArray(check, template, NB_KEYWORDS)) {
	printf("Error parsing config.txt: default values loaded\n");
	screenWidth = 1200;
	screenHeight = 800;
	setColor(&background, 128, 128, 128);
	setColor(&untextured, 0, 255, 255);
	initMultimedia("multimedia/libmultimedia_SDL.so");
	initDisplay(screenWidth, screenHeight, &background, &untextured);
	scene.camera = initCamera("cameras/standard.txt");
	freeLightBuffer();
	addLightToScene(loadLight("light/standard.txt"));
    } else {
	initMultimedia(multimedia);
	initDisplay(screenWidth, screenHeight, &background, &untextured);
	scene.camera = initCamera(camera);
    }
    refreshCamera(scene.camera, screenWidth, screenHeight);
}

void removeSolidFromScene()
{
    if(scene.nbSolid > 0)
	freeSolid(scene.solidBuffer[--scene.nbSolid]);
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
    
void drawScene(void)
{
    Camera *C = scene.camera;
    int nbLens = getNbLens(C);
    Color color;

    resetCamera(C);
    resetDisplay();
    for (int j = 0; j < nbLens; j++) {
	if (getStateCamera(C, DRAW))
	    for (int i = 0; i < scene.nbSolid; i++)
		drawSolid(getLensOfCamera(C, j), scene.solidBuffer[i]);
	if (getStateCamera(C, WIREFRAME))
	    for (int i = 0; i < scene.nbSolid; i++)
		wireframeSolid(getLensOfCamera(C, j), scene.solidBuffer[i], 
			       setColor(&color, 255, 0, 0));
	if (getStateCamera(C, NORMAL))
	    for (int i = 0; i < scene.nbSolid; i++)
		normalSolid(getLensOfCamera(C, j), scene.solidBuffer[i], 
			    setColor(&color, 0, 255, 0));
	if (getStateCamera(C, VERTEX))
	    for (int i = 0; i < scene.nbSolid; i++)
		vertexSolid(getLensOfCamera(C, j), scene.solidBuffer[i], 
			    setColor(&color, 0, 0, 255));
	if (getStateCamera(C, FRAME))
	    drawFrame(getLensOfCamera(C, j), &scene.origin);
    }
    blitDisplay();
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

void calculateLightScene(const Point *A, const Point *nA, Color *c)
{
    setColor(c, 0, 0, 0);
    for (int i = 0; i < scene.nbLight; i++) {
	Color tmp;
	calculateLight(scene.lightBuffer[i], A, nA, &tmp);
	sumColor(c, &tmp, c);
    }
}

void resizeCameraScene(int screenWidth, int screenHeight)
{
    resizeDisplay(screenWidth, screenHeight);
    refreshCamera(scene.camera, screenWidth, screenHeight);
}

void rotateCameraScene(int direction)
{
    rotateCamera(scene.camera, direction);
}

void translateCameraScene(int direction)
{
    translateCamera(scene.camera, direction);
}

void switchStateCameraScene(int state)
{
    switchStateCamera(scene.camera, state);
}

void freeScene(void)
{
    freeSolidBuffer();
    freeLightBuffer();
    freeCamera(scene.camera);
    freeDisplay();
    freeMultimedia();
}
