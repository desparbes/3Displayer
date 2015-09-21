#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <readline/readline.h>

#include "scene.h"

int main(int argc, char *argv[])
{
    rl_bind_key('\t', rl_complete);
    initScene();
    handleArgumentScene(argc, argv);
    
    int stop = 0;
    while (!stop) {
	updateScene(&stop);
	drawScene();
    }
    
    freeScene();
    return EXIT_SUCCESS;
}
