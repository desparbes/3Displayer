#ifndef POSITION_H
#define POSITION_H

typedef struct Position {
    float x;
    float y;
} Position;

void loopPosition(Position *t);
void setPosition(Position *t, float x, float y);

#endif // POSITION_H
