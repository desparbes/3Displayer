#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#include "scene.h"
#include "solid.h"

int main(int argc, char *argv[])
{
    Solid *solid = initSolid();

    switch (argc) {
    case 1:
	fprintf(stderr, "Usage: ./3Displayer .OBJ_PATH (.BMP_PATH)\n");
	exit(EXIT_FAILURE);
	break;
    case 2:
	loadSolid(solid, argv[1], NULL);
	break;
    default:
	loadSolid(solid, argv[1], argv[2]);
	break;
    }

    int stop = 0;
    initScene();
    addSolidToScene(solid);
    while (!stop) {
	updateScene(&stop);
	drawScene();
    }
    freeScene();
    return EXIT_SUCCESS;
}
