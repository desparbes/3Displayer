#include "SDL/SDL.h"
#include "scene.h"
#include "frame.h"
#include "camera.h"

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
	rotateCamera(getCamera(), state.theta, state.phi, state.rho);
    }
    state.mouseWidth = event->motion.x;
    state.mouseHeight = event->motion.y;
}

void handleMouseButtonUpEvent(SDL_Event *event)
{
    switch (event->button.button) {
    case SDL_BUTTON_RIGHT:
	state.rightClickDown = 0;
	break;
    default:
	break;
    }
}

void handleMouseButtonDownEvent(SDL_Event *event)
{
    Camera *c = getCamera();
    Frame *cPosition = getPositionCamera(c);

    switch (event->button.button) {
    case SDL_BUTTON_RIGHT:
	state.rightClickDown = 1;
	break;
    case SDL_BUTTON_WHEELUP:
	translateCamera(c, 
		       state.translationSpeed * cPosition->j.x,
		       state.translationSpeed * cPosition->j.y,
		       state.translationSpeed * cPosition->j.z);
	break;
    case SDL_BUTTON_WHEELDOWN:
	translateCamera(getCamera(), 
		       -state.translationSpeed * cPosition->j.x,
		       -state.translationSpeed * cPosition->j.y,
		       -state.translationSpeed * cPosition->j.z);
	break;
    default:
	break;
    }
}

void handleKeyDownEvent(SDL_Event *event)
{
    SDL_Event q;
    Camera *c = getCamera();
    Frame *cPosition = getPositionCamera(c);
    
    switch (event->key.keysym.sym) {
    case SDLK_LEFT:
	translateCamera(c, 
		       -state.translationSpeed * cPosition->i.x,
		       -state.translationSpeed * cPosition->i.y,
		       -state.translationSpeed * cPosition->i.z);
	break;
    case SDLK_RIGHT:
	translateCamera(c, 
		       state.translationSpeed * cPosition->i.x,
		       state.translationSpeed * cPosition->i.y, 
		       state.translationSpeed * cPosition->i.z);
	break;
    case SDLK_UP:
	translateCamera(c, 
		       state.translationSpeed * cPosition->k.x,
		       state.translationSpeed * cPosition->k.y, 
		       state.translationSpeed * cPosition->k.z);
	break;
    case SDLK_DOWN:
	translateCamera(c, 
		       -state.translationSpeed * cPosition->k.x,
		       -state.translationSpeed * cPosition->k.y, 
		       -state.translationSpeed * cPosition->k.z);
	break;
    case SDLK_ESCAPE:
	q.type = SDL_QUIT;
	SDL_PushEvent(&q);
	break;
    default:
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
    case SDLK_l:
	askSolidForScene();
	break;
    case SDLK_u:
	removeSolidFromScene();
	break;
    case SDLK_e:
	askEquationForScene();
	break;
    default:
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
