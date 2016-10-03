#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "position.h"

void test_setPosition()
{
    Position p;
    setPosition(&p, 0.5, 0.);
    assert(p.x == 0.5 && p.y == 0.);
}

void test_loopPosition(void)
{
    Position p;

    setPosition(&p, 0.5, 0.);
    loopPosition(&p);
    assert(p.x == 0.5 && p.y == 0.);

    setPosition(&p, -0.1, 1.2);
    loopPosition(&p);
    assert(p.x < 1. && p.x >= 0. && p.y < 1. && p.y >= 0.);

    setPosition(&p, -0., 1.);
    loopPosition(&p);
    assert(p.x == 0. && p.y == 0.);
}

int main(int argc, char *argv[])
{
    test_setPosition();
    test_loopPosition();
    return EXIT_SUCCESS;
}
