static struct {
    Frame camera;
    Frame origin;

    float translationSpeed;
    float rotationSpeed;
    
    int draw; // boolean : 1 draw the scene, else don't
    int wireframe; // boolean : 1 wireframe the scene, else don't
    int normal; // boolean : 1 display the normals of scene, else don't
    int vertex; // boolean : 1 display the vertex of the scene, else don't
    int origin; // boolean : 1 display the origin of the scene, else dont't

    Point light;

    Solid **solidBuffer;
    int nbSolid;
    int bufferSize;

    SDL_Surface *screen;
    int wfov;
    int hfov;
    int screenWidth;
    int screenHeight;
    float nearplan;
    float farplan;
    float *zBuffer
} scene;

static void resetZBuffer(float *zBuffer)
{
    int size = WIDTH * HEIGHT;
    for (int i = 0; i < size; i++)
	zBuffer[i] = -1.;
}

static void resetFrame(Frame *user)
{
    setPoint(&user->position, 0., 0., 0.);
    setPoint(&user->i, 1., 0., 0.);
    setPoint(&user->j, 0., 1., 0.);
    setPoint(&user->k, 0., 0., 1.);
}

static void initLight(Point *light, float x, float y, float z)
{
    setPoint(light, x, y, z);
    normalizePoint(light, light);
}

static void initSDL(SDL_Surface *screen, int sreenWidth, int screenHeight)
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
	fprintf(stderr, "Error SDL_Init: %s", SDL_GetError());
	exit(EXIT_FAILURE);
    }
    if((screen = SDL_SetVideoMode(sreenWidth, sreenHeight, 64,
				  SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL) {
	fprintf(stderr, "Error SDL_SetVideoMode: %s", SDL_GetError());
	exit(EXIT_FAILURE);
    }
    SDL_WM_SetCaption("3Displayer", NULL);
    SDL_EnableKeyRepeat(1, 10);
}

static void handleMouseEvent(SDL_Event *event)
{
    static float theta = 0., phi = 0., rho = 0.;
    static int mouseWidth = 0, mouseHeight = 0;
    static int rightClickDown = 0;
    switch (event->type) {
    case SDL_MOUSEMOTION:
	if (rightClickDown) {
	    if (mouseWidth > event->motion.x)
		theta -= scene.rotationSpeed;
	    else if (mouseWidth < event->motion.x)
		theta += scene.rotationSpeed;
	    else if (mouseHeight > event->motion.y)
		phi += scene.rotationSpeed;
	    else if (mouseHeight < event->motion.y)
		phi -= scene.rotationSpeed;
	}
	mouseWidth = event->motion.x;
	mouseHeight = event->motion.y;
	break;
    case SDL_MOUSEBUTTONUP:
	switch (event->button.button) {
	case SDL_BUTTON_RIGHT:
	    rightClickDown = 0;
	    break;
	}
	break;
    case SDL_MOUSEBUTTONDOWN:
	switch (event->button.button) {
	case SDL_BUTTON_RIGHT:
	    rightClickDown = 1;
	    break;
	}
	break;
    }
    direction(theta, phi, rho);
}

static void handleKeyBordEvent(SDL_Event *event, int *stop)
{
    switch (event->key.keysym.sym) {
    case SDLK_LEFT:
	translatePoint(&scene.camera.O, 
		       -scene.translationSpeed * scene.camera.i.x,
		       -scene.translationSpeed * scene.camera.i.y,
		       -scene.translationSpeed * scene.camera.i.z);
	break;
    case SDLK_RIGHT:
	translatePoint(&scene.camera.O, 
		       scene.translationSpeed * scene.camera.i.x,
		       scene.translationSpeed * scene.camera.i.y, 
		       scene.translationSpeed * scene.camera.i.z);
	break;
    case SDLK_UP:
	translatePoint(&scene.camera.O, 
		       scene.translationSpeed * scene.camera.j.x,
		       scene.translationSpeed * scene.camera.j.y, 
		       scene.translationSpeed * scene.camera.j.z);
	break;
    case SDLK_DOWN:
	translatePoint(&scene.camera.O, 
		       -scene.translationSpeed * scene.camera.j.x,
		       -scene.translationSpeed * scene.camera.j.y, 
		       -scene.translationSpeed * scene.camera.j.z);
	break;
    case SDLK_KP_PLUS:
	translatePoint(&scene.camera.O, 
		       scene.translationSpeed * scene.camera.k.x,
		       scene.translationSpeed * scene.camera.k.y, 
		       scene.translationSpeed * scene.camera.k.z);
	break;
    case SDLK_KP_MINUS:
	translatePoint(&scene.camera.O, 
		       -scene.translationSpeed * scene.camera.k.x,
		       -scene.translationSpeed * scene.camera.k.y, 
		       -scene.translationSpeed * scene.camera.k.z);
	break;
    case SDLK_ESCAPE:
	*stop = 1;
	break;
    case SDLK_r:
	initCamera();
	break;
    default:
	break;
    }
}

static void handleEvent(SDL_Event *event, int *stop)
{
    switch (event->type) {
    case SDL_QUIT:
	*stop = 1;
	break;
    case SDL_KEYDOWN:
	handleKeyBoardEvent(event, stop);
	break;
    case SDL_MOUSEMOTION:
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
	handleMouseEvent(event);
	break;
    default:
	break;
    }
}

void initScene()
{
    scene.translationSpeed = 0.1;
    scene.rotationSpeed = 0.01;

    scene.draw = 1;
    scene.wireframe = 0;
    scene.normal = 0;
    scene.vertex = 0;
    scene.origin = 1;

    scene.nbSolid = 0;
    scene.bufferSize = 4;
    scene.wfov = 80;
    scene.hfov = 60;
    scene.screenWidth = 1200;
    scene.screenHeight = 900;
    scene.nearplan = 1.;
    scene.farplan = 20.;
    
    initLight(&scene.light, 1., -0.5, -2.);
    resetFrame(&scene.camera);
    resetFrame(&scene.origin);

    scene.solidBuffer = malloc(scene.bufferSize * sizeof(Solid*));
    scene.zBuffer = malloc(scene.screenWidth * scene.screenHeight * 
			   sizeof(float));
    initSDL(scene.screen, scene.screenWidth, scene.screenHeight);
}

void updateScene(int *stop)
{
    SDL_Event event;
    SDL_WaitEvent(&event);
    handleEvent(&event, stop);    
}

void addSolidToScene(Solid *solid)
{
    if(scene.nbSolid >= scene.bufferSize){
	scene.bufferSize *= 2;
	scene.solidBuffer = realloc(scene.solidBuffer, 
				    scene.bufferSize * 
				    sizeof(Solid *));
    }
    scene.solidBuffer[scene.nbSolid] = solid;
    scene.nbSolid++;    
}

void removeSolidFromScene(Solid *solid)
{
    for (int i = 0; i < scene.nbSolid && scene.solidBuffer[i] != solid; i++);
    scene.solidBuffer[i] = scene.solidBuffer[--scene.nbSolid];
}
    
void drawScene()
{
    SDL_FillRect(scene.screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    resetZBuffer(scene.zBuffer);
    drawOrigin();
    
    if (scene.draw)
	for (int i = 0; i < scene.nbSolid; i++)
	    drawSolid(scene.solidBuffer[i]);
    if (scene.wireframe)
	for (int i = 0; i < scene.nbSolid; i++)
	    wireframeSolid(scene.solidBuffer[i], &red);
    if (scene.normal)
	for (int i = 0; i < scene.nbSolid; i++)
	    normalSolid(scene.solidBuffer[i], &green);
    if (scene.vertex)
	for (int i = 0; i < scene.nbSolid; i++)
	    vertexSolid(scene.solidBuffer[i], &blue);
    if (scene.origin)
	drawOrigin();
    SDL_Flip(screen);
}

void freeScene()
{
    for(int i = 0; i < scene.nbSolid; i++)
	freeSolid(&scene.solidBuffer[i]);
    free(scene.solidBuffer);
    free(scene.zBuffer);
    SDL_FreeSurface(scene.screen);
    SDL_Quit();
}
