#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>

typedef struct Window_ Window;
typedef struct Event_ Event;

#include "color.h"
#include "coord.h"
#include "config.h"
#include "camera.h" // direction_t

struct Window_ {
    void (*free)(Window*);

    void (*reset)(Window*);
    void (*update)(Window*);

    int (*getWidth)(Window*);
    int (*getHeight)(Window*);

    void (*setPixel)(Window*, Coord, Color);
    Color (*getPixel)(Window*, Coord);

    bool (*pollEvent)(Window*, Event *event);
    void (*waitEvent)(Window*, Event *event);
};

typedef enum event_type {
    QUIT,
    ROTATE,
    TRANSLATE,
    STATE,
    RESIZE,
    LOAD,
    UNLOAD,
    EXPOSED,
    NO_EVENT,
} event_type;

struct Event_ {
    event_type type;
    direction_t direction;
    state_t state;
    int width, height;
};

void initWindow(Config *config);
bool handleEventWindow(void);
void freeWindow(void);
Color getPixelWindow(Coord pos);

void resetWindow(void);
void updateWindow(void);
void setPixelWindow(Coord coord, Color color);

#endif //WINDOW_H
