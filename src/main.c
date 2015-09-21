#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <readline/readline.h>

#include "scene.h"

int main(int argc, char *argv[])
{
    int stop = 0;
    rl_bind_key('\t',rl_complete);
    initScene();
    handleArgumentScene(argc, argv);
    while (!stop) {
	updateScene(&stop);
	drawScene();
    }
    freeScene();
    return EXIT_SUCCESS;
}
