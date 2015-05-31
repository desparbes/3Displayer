typedef struct State State;

State *initState(int nbState);
void resetState(State *state, int value);
void setState(State *state, int value, int id);
int getState(State *state, int id);
int analyseState(State *state, int value);
void destroyState(State *state);
