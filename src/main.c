#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#include "scene.h"
#include "solid.h"

void handleArgument(int argc, char *argv[])
{
    Solid *solid;
    switch (argc) {
    case 1:
	break;
    case 2:
	if (solid = loadSolid(argv[1], NULL))
	    addSolidToScene(solid);
	break;
    default:
	if (solid = loadSolid(argv[1], argv[2]))
	    addSolidToScene(solid);
	break;
    }
}

int main(int argc, char *argv[])
{
    int stop = 0;
    initScene();
    handleArgument(argc, argv);
    while (!stop) {
	updateScene(&stop);
	drawScene();
    }
    freeScene();
    return EXIT_SUCCESS;
}
