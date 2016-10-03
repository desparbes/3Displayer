#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scene.h"
#include "point.h"
#include "frame.h"
#include "solid.h"
#include "color.h"
#include "camera.h"
#include "window.h"
#include "array.h"
#include "light.h"
#include "buffer.h"

#define MAXLENGTH 128

enum keywords {
    SCREENWIDTH,
    SCREENHEIGHT,
    BACKGROUND,
    UNTEXTURED,
    MULTIMEDIA,
    CAMERA,
    NB_KEYWORDS,
};

typedef struct Scene Scene;

struct Scene {
    Frame origin;

    Light **lightBuffer;
    int nbLight;
    int lightSize;

    Solid **solidBuffer;
    int nbSolid;
    int solidSize;

    Camera *camera;
};


static Scene scene;

static void removeSpace(char *buf)
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
    addElementToBuffer(solid, scene.solidBuffer,
                       &scene.solidSize, &scene.nbSolid);
    if (solid)
        calculateOriginSolid(solid);
}

static void addLightToScene(Light *light)
{
    addElementToBuffer(light, scene.lightBuffer,
                       &scene.lightSize, &scene.nbLight);
}

static void freeSolidBuffer(void)
{
    for(int i = 0; i < scene.nbSolid; i++)
        freeSolid(scene.solidBuffer[i]);
    g_free(scene.solidBuffer);
}

static void freeLightBuffer(void)
{
    for(int i = 0; i < scene.nbLight; i++)
        freeLight(scene.lightBuffer[i]);
    g_free(scene.lightBuffer);
}

void initScene(Config *config)
{
    memset(&scene, 0, sizeof scene);
    initFrame(&scene.origin);
    scene.camera = NULL;
    scene.nbSolid = 0;
    scene.solidSize = 4;
    scene.solidBuffer = g_malloc(scene.solidSize * sizeof scene.solidBuffer[0]);
    scene.nbLight = 0;
    scene.lightSize = 4;
    scene.lightBuffer = g_malloc(scene.lightSize * sizeof scene.lightBuffer[0]);
    scene.camera = initCamera("cameras/standard.txt");
    addLightToScene(loadLight("light/standard.txt"));
    refreshCamera(scene.camera, config->window_width, config->window_height);
}

void removeSolidFromScene(void)
{
    if (scene.nbSolid > 0) {
        freeSolid(scene.solidBuffer[--scene.nbSolid]);
        printf("Solid successfully removed\n");
    }
}

void askSolidForScene(void)
{
    char *tex_path = NULL;
    char *obj_path = NULL;
    size_t size = 0;

    printf("Solid path: ");
    getline(&obj_path, &size, stdin);
    removeSpace(obj_path);

    printf("Bitmap (texture) path: ");
    getline(&tex_path, &size, stdin);
    removeSpace(tex_path);

    addSolidToScene(loadSolid(obj_path, tex_path));
    free(obj_path);
    free(tex_path);
}

void drawScene(void)
{
    Camera *C = scene.camera;
    int nbLens = getNbLens(C);
    Color red=COLOR(255,0,0), blue=COLOR(0,255,0), green=COLOR(0,0,255);

    resetCamera(C);
    for (int j = 0; j < nbLens; j++) {
        if (getStateCamera(C, DRAW)) {
            for (int i = 0; i < scene.nbSolid; i++)
                drawSolid(getLensOfCamera(C, j), scene.solidBuffer[i]);
        }
        if (getStateCamera(C, WIREFRAME)) {
            for (int i = 0; i < scene.nbSolid; i++)
                wireframeSolid(getLensOfCamera(C, j), scene.solidBuffer[i], red);
        }
        if (getStateCamera(C, NORMAL)) {
            for (int i = 0; i < scene.nbSolid; i++)
                normalSolid(getLensOfCamera(C, j), scene.solidBuffer[i], blue);
        }
        if (getStateCamera(C, VERTEX)) {
            for (int i = 0; i < scene.nbSolid; i++)
                vertexSolid(getLensOfCamera(C, j), scene.solidBuffer[i], green);
        }
        if (getStateCamera(C, FRAME))
            drawFrame(getLensOfCamera(C, j), &scene.origin);
    }
}

void handleArgumentScene(int argc, char *argv[])
{
    if (argc == 2)
        addSolidToScene(loadSolid(argv[1], NULL));
    else if (argc == 3)
        addSolidToScene(loadSolid(argv[1], argv[2]));
}

void calculateLightScene(const Point *A, const Point *nA, Color *c)
{
    SET_COLOR(*c, 0, 0, 0);
    for (int i = 0; i < scene.nbLight; i++) {
        Color tmp;
        calculateLight(scene.lightBuffer[i], A, nA, &tmp);
        ADD_COLOR(*c, tmp, *c);
    }
}

void resizeCameraScene(int screenWidth, int screenHeight)
{
    resizeWindow(screenWidth, screenHeight);
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
}
