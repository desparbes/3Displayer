#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "coord.h"
#include "color.h"
#include "position.h"

#define toString(x) #x
#define append(x, y) toString(x ## y)
#define loadFunction(x) loadFunction_(x, append(x, _))

static void *handle;

void (*initDisplay)(int, int, const Color *);
void (*resetDisplay)();
void (*pixelDisplay)(const Coord *, const Color *);
void (*blitDisplay)();
void (*freeDisplay)();

typedef struct Texture Texture;

Texture *(*loadTexture)(const char *);
void (*getPixelTexture)(const Texture *, const Position *, Color *);
void (*freeTexture)(Texture *);

void (*handleEvent)(int *);

static void loadFunction_(void *funAddress, const char *symbol)
{
    funAddress = dlsym(handle, symbol);
    if (funAddress == NULL) {
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
    loadFunction(resetDisplay);
    loadFunction(pixelDisplay);
    loadFunction(blitDisplay);
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
