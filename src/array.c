#include <stdio.h>

void initArray(int array[], int arraySize, int value)
{
    for (int i = 0; i < arraySize; i++)
	array[i] = value;
}

int areEqualsArray(int a[], int b[], int arraySize)
{
    for (int i = 0; i < arraySize; i++) {
	if (a[i] != b[i])
	    return 0;
    }
    return 1;
}
