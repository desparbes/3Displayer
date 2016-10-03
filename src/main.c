#include <stdio.h>
#include <stdlib.h>

#include "scene.h"
#include "window.h"
#include "config.h"
#include "draw.h"

int main(int argc, char *argv[])
{
    Config c;
    parseConfig(&c, argc, argv);

    initWindow(&c);
    initScene(&c);
    initRenderer(&c);
    handleArgumentScene(argc, argv);

    int stop = 0;
    while (!stop) {
        resetWindow();
        drawScene();
        updateWindow();
        stop = handleEventWindow();
    }
    freeScene();
    freeWindow();
    return EXIT_SUCCESS;
}
