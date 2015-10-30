#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "coord.h"
#include "color.h"
#include "position.h"

#define loadFunction(x) loadFunction_((void **)&x, #x)

static void *handle;

void (*initDisplay)(int, int, const Color *);
void (*resizeDisplay)(int, int);
void (*resetDisplay)();
void (*pixelDisplay)(const Coord *, const Color *);
void (*blitDisplay)();
int (*getWidthDisplay)();
int (*getHeightDisplay)();
void (*freeDisplay)();

typedef struct Texture Texture;

Texture *(*loadTexture)(const char *);
void (*getPixelTexture)(const Texture *, const Position *, Color *);
void (*freeTexture)(Texture *);

void (*handleEvent)(int *);

static void loadFunction_(void **funAddress, const char *symbol)
{
    *funAddress = *(void **)dlsym(handle, symbol);
    if (*funAddress == NULL) {
	puts(dlerror());
	exit(EXIT_FAILURE);
    }
}

void initMultimedia(const char *libPath)
{
    if (!(handle = dlopen(libPath, RTLD_NOW))) {
	puts(dlerror());
	exit(EXIT_FAILURE);
    }

    loadFunction(initDisplay);
    loadFunction(resizeDisplay);
    loadFunction(resetDisplay);
    loadFunction(pixelDisplay);
    loadFunction(blitDisplay);
    loadFunction(getWidthDisplay);
    loadFunction(getHeightDisplay);
    loadFunction(freeDisplay);

    loadFunction(loadTexture);
    loadFunction(getPixelTexture);
    loadFunction(freeTexture);

    loadFunction(handleEvent);
}

void freeMultimedia(void)
{
    dlclose(handle);
}
