#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>

#include "scene.h"
#include "event.h"

int main(int argc, char *argv[])
{
    initScene();
    handleArgumentScene(argc, argv);
    rl_bind_key('\t', rl_complete);
    
    int stop = 0;
    while (!stop) {
	handleEvent(&stop);
	drawScene();
    }
    
    freeScene();
    return EXIT_SUCCESS;
}
