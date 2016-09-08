#ifndef CONFIG_H
#define CONFIG_H

#include "color.h"

typedef struct Config {
    char *scene_camera;
    char *window_backend;
    int window_height;
    int window_width;
    Color window_background;
    Color renderer_untextured;
} Config;

void parseConfig(Config *c, int argc, char **argv);

#endif //CONFIG_H
