#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"

void parseConfig(Config *config, int argc, char **argv)
{
    memset(config, 0, sizeof *config);

    GKeyFile *k;
    k = g_key_file_new();
    if (!g_key_file_load_from_file(k, "config/config.ini", G_KEY_FILE_NONE, NULL)) {
        fprintf(stderr, "Cannot load config file\n");
        exit(EXIT_FAILURE);
    }
    config->window_width = g_key_file_get_integer(k, "window", "width", NULL);
    config->window_height = g_key_file_get_integer(k, "window", "height", NULL);
    config->window_backend = g_key_file_get_string(k, "window", "backend", NULL);

    gsize length;
    int *c;
    c = g_key_file_get_integer_list(k, "renderer", "untextured", &length, NULL);
    g_assert(length == 3);
    SET_COLOR(config->renderer_untextured, c[0], c[1], c[2]);
    g_free(c);

    c = g_key_file_get_integer_list(k, "window", "background", &length, NULL);
    g_assert(length == 3);
    SET_COLOR(config->window_background, c[0], c[1], c[2]);
    g_free(c);

    g_key_file_unref(k);
}
