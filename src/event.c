#include "SDL/SDL.h"
#include "scene.h"
#include "frame.h"

static struct {
    int draw; // boolean : 1 draw the scene, else don't
    int wireframe; // boolean : 1 wireframe the scene, else don't
    int normal; // boolean : 1 display the normals of scene, else don't
    int vertex; // boolean : 1 display the vertex of the scene, else don't
    int frame; // boolean : 1 display the frame of the scene, else dont't
    int rightClickDown;

    float theta;
    float phi;
    float rho;
    int mouseWidth;
    int mouseHeight;
    float translationSpeed;
    float rotationSpeed;
} state;

void resetEvent()
{
    state.draw = 1;
    state.wireframe = 0;
    state.normal = 0;
    state.vertex = 0;
    state.frame = 1;
    state.rightClickDown = 0;
    state.theta = 0.;
    state.phi = 0.;
    state.rho = 0.;
    state.mouseWidth = 0;
    state.mouseHeight = 0;
    state.translationSpeed = 0.1;
    state.rotationSpeed = 0.01;
}
    

void handleMouseMotionEvent(SDL_Event *event)
{
    if (state.rightClickDown) {
	if (state.mouseWidth > event->motion.x)
	    state.theta -= state.rotationSpeed;
	else if (state.mouseWidth < event->motion.x)
	    state.theta += state.rotationSpeed;
	else if (state.mouseHeight > event->motion.y)
	    state.phi += state.rotationSpeed;
	else if (state.mouseHeight < event->motion.y)
	    state.phi -= state.rotationSpeed;
    }
    rotateFrame(getCamera(), state.theta, state.phi, state.rho);
    state.mouseWidth = event->motion.x;
    state.mouseHeight = event->motion.y;
}

void handleMouseButtonUpEvent(SDL_Event *event)
{
    switch (event->button.button) {
    case SDL_BUTTON_RIGHT:
	state.rightClickDown = 0;
	break;
    }
}

void handleMouseButtonDownEvent(SDL_Event *event)
{
    switch (event->button.button) {
    case SDL_BUTTON_RIGHT:
	state.rightClickDown = 1;
	break;
    }
}

void handleKeyDownEvent(SDL_Event *event, int *stop)
{
    switch (event->key.keysym.sym) {
    case SDLK_LEFT:
	translateFrame(getCamera(), 
		       -state.translationSpeed * getCamera()->i.x,
		       -state.translationSpeed * getCamera()->i.y,
		       -state.translationSpeed * getCamera()->i.z);
	break;
    case SDLK_RIGHT:
	translateFrame(getCamera(), 
		       state.translationSpeed * getCamera()->i.x,
		       state.translationSpeed * getCamera()->i.y, 
		       state.translationSpeed * getCamera()->i.z);
	break;
    case SDLK_UP:
	translateFrame(getCamera(), 
		       state.translationSpeed * getCamera()->j.x,
		       state.translationSpeed * getCamera()->j.y, 
		       state.translationSpeed * getCamera()->j.z);
	break;
    case SDLK_DOWN:
	translateFrame(getCamera(), 
		       -state.translationSpeed * getCamera()->j.x,
		       -state.translationSpeed * getCamera()->j.y, 
		       -state.translationSpeed * getCamera()->j.z);
	break;
    case SDLK_KP_PLUS:
	translateFrame(getCamera(), 
		       state.translationSpeed * getCamera()->k.x,
		       state.translationSpeed * getCamera()->k.y, 
		       state.translationSpeed * getCamera()->k.z);
	break;
    case SDLK_KP_MINUS:
	translateFrame(getCamera(), 
		       -state.translationSpeed * getCamera()->k.x,
		       -state.translationSpeed * getCamera()->k.y, 
		       -state.translationSpeed * getCamera()->k.z);
	break;
    case SDLK_ESCAPE:
	*stop = 1;
	break;
    }
}

void handleKeyUpEvent(SDL_Event *event)
{
    switch (event->key.keysym.sym) {
    case SDLK_d:
	state.draw = (state.draw + 1) % 2;
	break;
    case SDLK_w:
	state.wireframe = (state.wireframe + 1) % 2;
	break;
    case SDLK_n:
	state.normal = (state.normal + 1) % 2;
	break;
    case SDLK_v:
	state.vertex = (state.vertex + 1) % 2;
	break;
   case SDLK_f:
	state.frame = (state.frame + 1) % 2;
	break;
    }
}

int getDrawEvent()
{
    return state.draw;
}

int getWireframeEvent()
{
    return state.wireframe;
}

int getNormalEvent()
{
    return state.normal;
}

int getVertexEvent()
{
    return state.vertex;
}

int getFrameEvent()
{
    return state.frame;
}
