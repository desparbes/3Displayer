typedef struct State State;

State *initState(int nbState);
void resetState(State *state);
void setRefuseState(State *state, int id);
int analyseState(State *state);
void destroyState(State *state);
