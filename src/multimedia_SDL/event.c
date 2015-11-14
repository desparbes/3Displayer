#include "SDL/SDL.h"
#include "scene.h"
#include "direction.h"
#include "view.h"

static struct {
    int rightClickDown;
    int mouseWidth;
    int mouseHeight;
} state;    

static void handleMouseMotionEvent(SDL_Event *event)
{
    if (state.rightClickDown) {
	if (state.mouseWidth > event->motion.x)
	    rotateCameraScene(LEFT);
	else if (state.mouseWidth < event->motion.x)
	    rotateCameraScene(RIGHT);
	else if (state.mouseHeight > event->motion.y)
	    rotateCameraScene(UP);
	else if (state.mouseHeight < event->motion.y)
	    rotateCameraScene(DOWN);
    }
    state.mouseWidth = event->motion.x;
    state.mouseHeight = event->motion.y;
}

static void handleMouseButtonUpEvent(SDL_Event *event)
{
    switch (event->button.button) {
    case SDL_BUTTON_RIGHT:
	state.rightClickDown = 0;
	break;
    default:
	break;
    }
}

static void handleMouseButtonDownEvent(SDL_Event *event)
{
    switch (event->button.button) {
    case SDL_BUTTON_RIGHT:
	state.rightClickDown = 1;
	break;
    case SDL_BUTTON_WHEELUP:
	translateCameraScene(FORWARD);
	break;
    case SDL_BUTTON_WHEELDOWN:
	translateCameraScene(BACKWARD);
	break;
    default:
	break;
    }
}

static void handleKeyDownEvent(SDL_Event *event)
{
    SDL_Event q;
    
    switch (event->key.keysym.sym) {
    case SDLK_q:
	translateCameraScene(LEFT);
	break;
    case SDLK_d:
	translateCameraScene(RIGHT);
	break;
    case SDLK_e:
	translateCameraScene(UP);
	break;
    case SDLK_a:
	translateCameraScene(DOWN);
	break;
    case SDLK_z:
	translateCameraScene(FORWARD);
	break;
    case SDLK_s:
	translateCameraScene(BACKWARD);
	break;
    case SDLK_LEFT:
	rotateCameraScene(LEFT);
	break;
    case SDLK_RIGHT:
	rotateCameraScene(RIGHT);
	break;
    case SDLK_UP:
	rotateCameraScene(UP);
	break;
    case SDLK_DOWN:
	rotateCameraScene(DOWN);
	break;
    case SDLK_ESCAPE:
	q.type = SDL_QUIT;
	SDL_PushEvent(&q);
	break;
    default:
	break;
    }
}

static void handleKeyUpEvent(SDL_Event *event)
{
    switch (event->key.keysym.sym) {
    case SDLK_o:
	switchStateCameraScene(DRAW);
	break;
    case SDLK_w:
	switchStateCameraScene(WIREFRAME);
	break;
    case SDLK_n:
	switchStateCameraScene(NORMAL);
	break;
    case SDLK_v:
	switchStateCameraScene(VERTEX);
	break;
   case SDLK_f:
	switchStateCameraScene(FRAME);
	break;
    case SDLK_l:
	askSolidForScene();
	break;
    case SDLK_u:
	removeSolidFromScene();
	break;
    default:
	break;
    }
}

void initEvent_(void){}

void handleEvent_(int *stop)
{
    SDL_Event event;
    while (!SDL_PollEvent(&event));
    
    do {
	switch (event.type) {
	case SDL_QUIT:
	    *stop = 1;
	    break;
	case SDL_VIDEORESIZE:
	    resizeCameraScene(event.resize.w, event.resize.h);
	    break;
	case SDL_KEYDOWN:
	    handleKeyDownEvent(&event);
	    break;
	case SDL_KEYUP:
	    handleKeyUpEvent(&event);
	    break;
	case SDL_MOUSEMOTION:
	    handleMouseMotionEvent(&event);
	    break;
	case SDL_MOUSEBUTTONUP:
	    handleMouseButtonUpEvent(&event);
	    break;
	case SDL_MOUSEBUTTONDOWN:
	    handleMouseButtonDownEvent(&event);
	    break;
	}
    } while (SDL_PollEvent(&event));
}

void freeEvent_(void){}

void (*initEvent)() = &initEvent_;
void (*handleEvent)(int *) = &handleEvent_;
void (*freeEvent)() = &freeEvent_;
