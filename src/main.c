#include <stdio.h>
#include <stdlib.h>

#include "scene.h"
#include "window.h"
#include "config.h"

int main(int argc, char *argv[])
{
    Config c;
    parseConfig(&c, argc, argv);
    
    initWindow(&c);
    initScene(&c);
    handleArgumentScene(argc, argv);

    int stop = 0;
    while (!stop) {
	stop = handleEventWindow();
        
        resetWindow();
	drawScene();
        updateWindow();
    }
    freeScene();
    freeWindow();
    return EXIT_SUCCESS;
}
