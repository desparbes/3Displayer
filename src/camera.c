#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lens.h"
#include "frame.h"
#include "coord.h"
#include "point.h"
#include "direction.h"
#include "view.h"

#define MAXLENGTH 256
#define NB_KEYWORDS 7

typedef struct {
    Frame position;
    float theta, phi, rho;
    Lens **lensBuffer;
    int bufferSize;
    int nbLens;
    int frequency;
    int state[NB_STATE];
    float rotationSpeed;
    float translationSpeed;
} Camera;

static void updateCamera(Camera *c)
{
    for (int i = 0; i < c->nbLens; i++)
	updateLens(c->lensBuffer[i], &c->position);
}

static void addLensToCamera(Camera *c, char *fileName)
{
    if(c->nbLens >= c->bufferSize){
	c->bufferSize *= 2;
	c->lensBuffer = realloc(c->lensBuffer, 
				c->bufferSize * 
				sizeof(Lens *));
    }
    c->lensBuffer[c->nbLens++] = initLens(fileName);
}

static void removeLensFromCamera(Camera *c)
{
    if (c->nbLens > 0)
	freeLens(c->lensBuffer[--c->nbLens]);
}

static void initStateCamera(Camera *c)
{
    for (int i = 0; i < NB_STATE; i++)
	c->state[i] = 0;
}

static void loadDefaultCamera(Camera *c)
{
    c->frequency = 1;
    c->translationSpeed = 0.1;
    c->rotationSpeed = 0.01;
    c->theta = 0.;
    c->phi = 0.;
    c->rho = 0.;
    setPoint(&c->position.O, 0., -5., 0.);
    for (int i = 0; i < c->nbLens; i++)
	removeLensFromCamera(c);
    addLensToCamera(c, "cameras/lens/standard.txt");
}

Camera *initCamera(char *fileName)
{
    Camera *c = malloc(sizeof(Camera));
    int checkCount = 0;
    initFrame(&c->position);
    initStateCamera(c);
    c->nbLens = 0;
    c->bufferSize = 2;
    c->lensBuffer = malloc(c->bufferSize * sizeof(Lens *));
    FILE *file = fopen(fileName, "r");

    if (file == NULL) {
	perror(fileName);
    } else {
	char str[MAXLENGTH];
	while (fscanf(file, "%s", str) != EOF) {
	    if (strcmp(str, "frequency") == 0 && 
		fscanf(file, "%d", &c->frequency) == 1)
		checkCount++;
	    else if (strcmp(str, "translationSpeed") == 0 &&
		     fscanf(file, "%f", &c->translationSpeed) == 1)
		checkCount++;
	    else if (strcmp(str, "rotationSpeed") == 0 &&
		     fscanf(file, "%f", &c->rotationSpeed) == 1)
		checkCount++;
	    else if (strcmp(str, "position") == 0 && 
		 fscanf(file, "(%f,%f,%f)", 
			&c->position.O.x, 
			&c->position.O.y,
			&c->position.O.z) == 3)
		checkCount++;
	    else if (strcmp(str, "theta") == 0 &&
		     fscanf(file, "%f", &c->theta) == 1)
		checkCount++;
	    else if (strcmp(str, "phi") == 0 &&
		 fscanf(file, "%f", &c->phi) == 1)
		checkCount++;
	    else if (strcmp(str, "rho") == 0 &&
		     fscanf(file, "%f", &c->rho) == 1)	    
		checkCount++;
	    else if (strcmp(str, "lens") == 0 &&
		     fscanf(file, "%s", str) == 1)	    
		addLensToCamera(c, str);
	}
    }
    if (checkCount != NB_KEYWORDS || c->nbLens == 0) {
	printf("Error parsing camera %s: default camera loaded\n", fileName);
        loadDefaultCamera(c);
    } else {
	rotateFrame(&c->position, c->theta, c->phi, c->rho);	
	printf("Camera %s successfully loaded\n", fileName);
    }
    return c;
}

void resetCamera(Camera *c)
{
    for (int i = 0; i < c->nbLens; i++)
	resetLens(c->lensBuffer[i]);
}

void translateCamera(Camera *c, int direction)
{
    switch(direction) {
    case BACKWARD:
	translateFrame(&c->position, &c->position.j, -c->translationSpeed);
	break; 
    case FORWARD:
	translateFrame(&c->position, &c->position.j, c->translationSpeed);
	break;
    case LEFT:
	translateFrame(&c->position, &c->position.i, -c->translationSpeed);
	break;
    case RIGHT:
	translateFrame(&c->position, &c->position.i, c->translationSpeed);
	break;
    case DOWN:
	translateFrame(&c->position, &c->position.k, -c->translationSpeed);
	break;
    case UP:
	translateFrame(&c->position, &c->position.k, c->translationSpeed);
	break;
    }
    updateCamera(c);
}

void rotateCamera(Camera *c, int direction)
{
    switch(direction) {
    case LEFT:
	c->theta -= c->rotationSpeed;
	break;
    case RIGHT:
	c->theta += c->rotationSpeed;
	break;
    case DOWN:
	c->phi -= c->rotationSpeed;
	break;
    case UP:
	c->phi += c->rotationSpeed;
	break;
    }	
    rotateFrame(&c->position, c->theta, c->phi, c->rho);
    updateCamera(c);
}

void switchStateCamera(Camera *c, int state)
{
    c->state[state] = (c->state[state] + 1) % 2;
}

int displayLensCamera(Camera *c, int lens)
{
    return 1;
}

Lens *getLensOfCamera(Camera *c, int lens)
{
    return c->lensBuffer[lens];
}

int getStateCamera(Camera *c, int state)
{
    return c->state[state];
}

int getNbLens(Camera *c)
{
    return c->nbLens;
}

void freeCamera(Camera *c)
{
    for (int i = 0; i < c->nbLens; i++)	
	freeLens(c->lensBuffer[i]);
    free(c->lensBuffer);
    free(c);
}
