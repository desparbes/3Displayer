#ifndef EVENT_H
#define EVENT_H

#include "SDL/SDL.h"

void resetEvent();
void handleMouseMotionEvent(SDL_Event *event);
void handleMouseButtonUpEvent(SDL_Event *event);
void handleMouseButtonDownEvent(SDL_Event *event);
void handleKeyDownEvent(SDL_Event *event, int *stop);
void handleKeyUpEvent(SDL_Event *event);

int getDrawEvent();
int getWireframeEvent();
int getNormalEvent();
int getVertexEvent();
int getFrameEvent();

#endif // EVENT_H
