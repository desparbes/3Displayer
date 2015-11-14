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
void (*getPixelDisplay)(const Coord *, Color *);
void (*setPixelDisplay)(const Coord *, const Color *);
void (*blitDisplay)();
void (*getUntexturedDisplay)(Color *);
int (*getWidthDisplay)();
int (*getHeightDisplay)();
void (*freeDisplay)();

void (*initEvent)();
void (*handleEvent)(int *);
void (*freeEvent)();

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
    loadFunction(getPixelDisplay);
    loadFunction(setPixelDisplay);
    loadFunction(blitDisplay);
    loadFunction(getUntexturedDisplay);
    loadFunction(getWidthDisplay);
    loadFunction(getHeightDisplay);
    loadFunction(freeDisplay);

    loadFunction(initEvent);
    loadFunction(handleEvent);
    loadFunction(freeEvent);
}

void freeMultimedia(void)
{
    dlclose(handle);
}
