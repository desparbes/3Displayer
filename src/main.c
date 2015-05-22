#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <math.h>

#include "project.h"
#include "data.h"
#include "basic.h"
#include "solid.h"
#include "draw.h"

void initZBuffer(float *zBuffer)
{
    int size = WIDTH * HEIGHT;
    for (int i = 0; i < size; i++)
	zBuffer[i] = -1.;
}

void initUserCamera(void)
{
    setPoint(&user.position, 0., -5., 0.);
    setPoint(&user.i, 1., 0., 0.);
    setPoint(&user.j, 0., 1., 0.);
    setPoint(&user.k, 0., 0., 1.);
}

void initScene(Solid *solid, const char *obj)
{
    normalizePoint(&light, &light);
    loadSolid(solid, obj);
    initUserCamera();
}

void handleMouseEvent(SDL_Event *event, Solid *solid)
{
    static float theta = 0., phi = 0., rho = 0.;
    static int width = 0, height = 0;
    switch (event->type) {
    case SDL_MOUSEMOTION:
	if (width > event->motion.x) {
	    theta -= ANGLE;
	} else if (width < event->motion.x) {
	    theta += ANGLE;
	}
	width = event->motion.x;
	if (height > event->motion.y) {
	    phi += ANGLE;
	} else if (height < event->motion.y) {
	    phi -= ANGLE;
	}
	height = event->motion.y;
	break;
    case SDL_MOUSEBUTTONUP:
	switch (event->button.button) {
	case SDL_BUTTON_WHEELDOWN:
	    //rho += ANGLE;
	    scaleSolid(solid, &PointO, 0.7);
	    break;
	case SDL_BUTTON_WHEELUP:
	    //rho -= ANGLE;
	    scaleSolid(solid, &PointO, 1.3);
	    break;
	case SDL_BUTTON_RIGHT:
	    theta = phi = rho = 0.;
	    break;
	}
	break;
    default:
	break;
    }
    direction(theta, phi, rho);
}

void handleKeyDownEvent(SDL_Event *event, int *stop)
{
    switch (event->key.keysym.sym) {
    case SDLK_LEFT:
	translatePoint(&user.position, -SPEED * user.i.x,
		       -SPEED * user.i.y, -SPEED * user.i.z);
	break;
    case SDLK_RIGHT:
	translatePoint(&user.position, SPEED * user.i.x,
		       SPEED * user.i.y, SPEED * user.i.z);
	break;
    case SDLK_UP:
	translatePoint(&user.position, SPEED * user.j.x,
		       SPEED * user.j.y, SPEED * user.j.z);
	break;
    case SDLK_DOWN:
	translatePoint(&user.position, -SPEED * user.j.x,
		       -SPEED * user.j.y, -SPEED * user.j.z);
	break;
    case SDLK_KP_PLUS:
	translatePoint(&user.position, SPEED * user.k.x,
		       SPEED * user.k.y, SPEED * user.k.z);
	break;
    case SDLK_KP_MINUS:
	translatePoint(&user.position, -SPEED * user.k.x,
		       -SPEED * user.k.y, -SPEED * user.k.z);
	break;
    case SDLK_ESCAPE:
	*stop = 1;
	break;
    case SDLK_r:
	initUserCamera();
	break;
    default:
	break;
    }
}

void handleEvent(SDL_Event *event, int *stop, Solid *solid)
{
    switch (event->type) {
    case SDL_QUIT:
	*stop = 1;
	break;
    case SDL_KEYDOWN:
	handleKeyDownEvent(event, stop);
	break;
    case SDL_MOUSEMOTION:
    case SDL_MOUSEBUTTONUP:
	handleMouseEvent(event, solid);
	break;
    default:
	break;
    }
}

void SDL_INIT()
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
	fprintf(stderr, "Error initializing SDL: %s", SDL_GetError());
	exit(EXIT_FAILURE);
    }
    if((screen = SDL_SetVideoMode(WIDTH, HEIGHT, BIT,
				  SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL) {
	fprintf(stderr, "SDL_SetVideoMode: %s", SDL_GetError());
	exit(EXIT_FAILURE);
    }
    SDL_WM_SetCaption("3Displayer", NULL);
    SDL_EnableKeyRepeat(1, 10);
    //SDL_ShowCursor(SDL_DISABLE);
}    

void drawScene(Solid *solid)
{
    //if(event.motion.x<20 ||
    //  event.motion.x>screen->w-20 || event.motion.y<20 ||
    // event.motion.y>screen->h-20)
    //   SDL_WarpMouse(screen->w / 2, screen->h / 2);
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    initZBuffer(zBuffer);
    //vertexSolid(&solid, red);
    //wireframeSolid(&solid, red);
    //segmentSolid(&solid, red);
    //drawSolid(solid);
    //normalSolid(solid, &green);
    //drawSolid(&lol);
    projectSegment(&PointO, &I, &red);
    projectSegment(&PointO, &J, &green);
    projectSegment(&PointO, &K, &blue);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, ".obj stp\n");
	exit(EXIT_FAILURE);
    }
    wCoef = WIDTH / (2 * tan(WFOV * M_PI / 360));
    hCoef = -HEIGHT / (2 * tan(HFOV * M_PI / 360));
    Solid solid;
    //Solid lol;
    initScene(&solid, argv[1]);
    SDL_INIT();
    zBuffer = malloc(WIDTH * HEIGHT * sizeof(float));
    initZBuffer(zBuffer);
    int stop = 0;
    SDL_Event event;
    while (!stop) {
	SDL_WaitEvent(&event);
	handleEvent(&event, &stop, &solid);
	drawScene(&solid);
	SDL_Flip(screen);
    }
    SDL_FreeSurface(screen);
    SDL_Quit();
    freeSolid(&solid);
    //freeSolid(&lol);
    free(zBuffer);
    return EXIT_SUCCESS;
}
