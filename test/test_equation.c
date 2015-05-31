#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../src/equation.c"

void test_analyseNumber(void)
{
    char *s = "lol16.125";
    int i = 3;
    float f = analyseNumber(s, &i);
    assert(f == 16.125 && i == 8);
    s = "16lol";
    i = 0;
    f = analyseNumber(s, &i);
    assert(f == 16. && i == 1);
}

int main(int argc, char *argv[])
{
    test_analyseNumber();
    return EXIT_SUCCESS;
}
