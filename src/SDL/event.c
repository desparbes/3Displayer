#include "SDL/SDL.h"
#include "scene.h"
#include "window.h"

struct sdl_state {
    int rightClickDown;
    int mouseX;
    int mouseY;
};
static struct sdl_state state;

static void handleMouseMotionEvent(Event *event, SDL_Event *sdl_event)
{
    event->type = ROTATE;
    if (state.rightClickDown) {
	if (sdl_event->motion.x < state.mouseX)
            event->direction = LEFT;
	else if (sdl_event->motion.x > state.mouseX)
            event->direction = RIGHT;
	else if (sdl_event->motion.y < state.mouseY)
            event->direction = UP;
	else if (sdl_event->motion.y > state.mouseY)
            event->direction = DOWN;
    }
    state.mouseX = sdl_event->motion.x;
    state.mouseY = sdl_event->motion.y;
}

static void handleMouseButtonUpEvent(SDL_Event *sdl_event)
{
    switch (sdl_event->button.button) {
    case SDL_BUTTON_RIGHT:
	state.rightClickDown = 0;
	break;
    default:
	break;
    }
}

static bool handleMouseButtonDownEvent(Event *event, SDL_Event *sdl_event)
{
    bool have_event = true;
    switch (sdl_event->button.button) {
    case SDL_BUTTON_RIGHT:
	state.rightClickDown = 1;
        have_event = false;
	break;
    case SDL_BUTTON_WHEELUP:
        event->type = TRANSLATE;
        event->direction = FORWARD;
	break;
    case SDL_BUTTON_WHEELDOWN:
        event->type = TRANSLATE;
        event->direction = BACKWARD;
	break;
    default:
	break;
    }
    return have_event;
}

static void handleKeyDownEvent(Event *event, SDL_Event *sdl_event)
{
    
    switch (sdl_event->key.keysym.sym) {
    case SDLK_q:
        event->type = TRANSLATE;
        event->direction = LEFT;
	break;
    case SDLK_d:
        event->type = TRANSLATE;
        event->direction = RIGHT;
	break;
    case SDLK_e:
        event->type = TRANSLATE;
        event->direction = UP;
	break;
    case SDLK_a:
        event->type = TRANSLATE;
        event->direction = DOWN;
	break;
    case SDLK_z:
        event->type = TRANSLATE;
        event->direction = FORWARD;
	break;
    case SDLK_s:
        event->type = TRANSLATE;
        event->direction = BACKWARD;
	break;
    case SDLK_LEFT:
        event->type = ROTATE;
        event->direction = LEFT;
	break;
    case SDLK_RIGHT:
        event->type = ROTATE;
        event->direction = RIGHT;
	break;
    case SDLK_UP:
        event->type = ROTATE;
        event->direction = UP;
	break;
    case SDLK_DOWN:
        event->type = ROTATE;
        event->direction = DOWN;
	break;
    case SDLK_ESCAPE:
        event->type = QUIT;
	break;
    default:
	break;
    }
}

static void handleKeyUpEvent(Event *event, SDL_Event *sdl_event)
{
    switch (sdl_event->key.keysym.sym) {
    case SDLK_o:
        event->type = STATE;
        event->state = DRAW;
	break;
    case SDLK_w:
        event->type = STATE;
        event->state = WIREFRAME;
	break;
    case SDLK_n:
        event->type = STATE;
        event->state = NORMAL;
	break;
    case SDLK_v:
        event->type = STATE;
        event->state = VERTEX;
	break;
    case SDLK_f:
        event->type = STATE;
        event->state = FRAME;
	break;
    case SDLK_l:
        event->type = LOAD;
	break;
    case SDLK_u:
        event->type = UNLOAD;
	break;
    default:
	break;
    }
}

bool PollEvent(Window *window, Event *event)
{
    SDL_Event sdl_event;
    if (!SDL_PollEvent(&sdl_event))
        return false;
    
    switch (sdl_event.type) {
    case SDL_QUIT:
        event->type = QUIT;
        break;
    case SDL_VIDEORESIZE:
        event->type = RESIZE;
        event->width = sdl_event.resize.w;
        event->height = sdl_event.resize.h;
        break;
    case SDL_KEYDOWN:
        handleKeyDownEvent(event, &sdl_event);
        break;
    case SDL_KEYUP:
        handleKeyUpEvent(event, &sdl_event);
        break;
    case SDL_MOUSEMOTION:
        handleMouseMotionEvent(event, &sdl_event);
        break;
    case SDL_MOUSEBUTTONUP:
        handleMouseButtonUpEvent(&sdl_event);
        return false;
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (!handleMouseButtonDownEvent(event, &sdl_event))
            return false;
        break;
    }
    return true;
}

