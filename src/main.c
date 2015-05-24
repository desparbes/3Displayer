#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#include "scene.h"
#include "solid.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "Usage: ./3Displayer .OBJ_PATH\n");
	exit(EXIT_FAILURE);
    }
    Solid *solid = initSolid();
    int stop = 0;
    initScene();
    loadSolid(solid, argv[1]);
    addSolidToScene(solid);
    while (!stop) {
	updateScene(&stop);
	drawScene();
    }
    freeScene();
    return EXIT_SUCCESS;
}
