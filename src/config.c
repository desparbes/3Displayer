#include <glib.h>
#include <string.h>
#include "config.h"

void parseConfig(Config *config, int argc, char **argv)
{
    memset(config, 0, sizeof *config);
    config->window_backend = g_strdup("SDL_backend");
    config->window_height = 640;
    config->window_width = 480;
    SET_COLOR(config->window_background, 128, 128, 128);
    SET_COLOR(config->renderer_untextured, 255, 255, 255);
}
