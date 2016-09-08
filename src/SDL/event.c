#include "SDL.h"
#include "scene.h"
#include "window.h"

struct sdl_state {
    int rightClickDown;
    int mouseX;
    int mouseY;
};
static struct sdl_state state;

static bool handleMouseMotionEvent(Event *event, SDL_Event *sdl_event)
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
    return true;
}

static bool handleMouseButtonUpEvent(SDL_Event *sdl_event)
{
    switch (sdl_event->button.button) {
    case SDL_BUTTON_RIGHT:
	state.rightClickDown = 0;
    default:
        return false;
    }
}

static bool handleMouseButtonDownEvent(SDL_Event *sdl_event)
{
    switch (sdl_event->button.button) {
    case SDL_BUTTON_RIGHT:
	state.rightClickDown = 1;
        return false;
    default:
        return false;
    }
    return false;
}

static bool handleMouseWheelEvent(Event *event, SDL_Event *sdl_event)
{
    event->type = TRANSLATE;
    if (sdl_event->wheel.y > 0)
        event->direction = FORWARD;
    else if (sdl_event->wheel.y < 0)
        event->direction = BACKWARD;
    else
        return false;
    return true;
}


static bool handleKeyDownEvent(Event *event, SDL_Event *sdl_event)
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
        return false;
    }
    return true;
}

static bool handleKeyUpEvent(Event *event, SDL_Event *sdl_event)
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
        return false;
    }
    return true;
}

static bool handleWindowEvent(Event *event, SDL_Event *sdl_event)
{
    switch(sdl_event->window.event) {
    case SDL_WINDOWEVENT_RESIZED:
        event->type = RESIZE;
        event->width = sdl_event->window.data1;
        event->height = sdl_event->window.data2;
        return true;
    default:
        return false;
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
        return true;
    case SDL_KEYDOWN:
        return handleKeyDownEvent(event, &sdl_event);
    case SDL_KEYUP:
        return handleKeyUpEvent(event, &sdl_event);
    case SDL_MOUSEMOTION:
        return handleMouseMotionEvent(event, &sdl_event);
    case SDL_MOUSEBUTTONUP:
        return handleMouseButtonUpEvent(&sdl_event);
    case SDL_MOUSEBUTTONDOWN:
        return handleMouseButtonDownEvent(&sdl_event);
    case SDL_MOUSEWHEEL:
        return handleMouseWheelEvent(event, &sdl_event);
    case SDL_WINDOWEVENT:
        return handleWindowEvent(event, &sdl_event);
    default:
        return false;
    }

    #ifdef __GNUC__
    __builtin_unreachable();
    #endif
}

