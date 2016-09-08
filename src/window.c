#include <stdio.h>
#include <stdlib.h>
#include <gmodule.h>
#include <glib/gi18n.h>
#include "window.h"
#include "scene.h"

static Window *window;
static GModule *sModule = NULL;

void initWindow(Config *config)
{
    char *path;
    if (sModule != NULL) {
        g_module_close(sModule);
        sModule = NULL;
    }

    path = g_module_build_path("multimedia", config->window_backend);
    if (path == NULL) {
	puts("Could not find window backend");
	exit(EXIT_FAILURE);
    }
    sModule = g_module_open(path, G_MODULE_BIND_LOCAL);
    g_free(path);

    if (sModule == NULL) {
	puts(g_module_error());
	exit(EXIT_FAILURE);
    }

    Window *(*CreateWindow)(int,int);
    if (!g_module_symbol(sModule, "CreateWindow", (gpointer*) &CreateWindow)) {
	puts(g_module_error());
	exit(EXIT_FAILURE);
    }

    window = CreateWindow(config->window_width, config->window_height);

    if (window == NULL) {
        fprintf(stderr, _("Could not create window!! aborting..."));
        exit(EXIT_FAILURE);
    }
}

bool handleEventWindow(void)
{
    Event event;
    while (!window->pollEvent(window, &event))
    { /* WAIT */ }

    do {
        switch (event.type) {
        case QUIT:
            return true;
        case TRANSLATE:
            translateCameraScene(event.direction);
            break;
        case ROTATE:
            rotateCameraScene(event.direction);
            break;
        case STATE:
            switchStateCameraScene(event.state);
            break;
        case RESIZE:
            resizeCameraScene(event.width, event.height);
            break;
        case LOAD:
            askSolidForScene();
            break;
        case UNLOAD:
            removeSolidFromScene();
            break;
        };
    } while (window->pollEvent(window, &event));
    return false;
}

void freeWindow(void)
{
    window->free(window);
    g_module_close(sModule);
    sModule = NULL;
}

void resetWindow(void)
{
    window->reset(window);
}

void updateWindow(void)
{
    window->update(window);
}

void setPixelWindow(Coord coord, Color color)
{
    window->setPixel(window, coord, color);
}

Color getPixelWindow(Coord coord)
{
    return window->getPixel(window, coord);
}

void resizeWindow(int width, int height)
{
    window->resize(window, width, height);
}
