#include <stdio.h>
#include <stdlib.h>

void addElementToBuffer(void *element, void **buffer, int *sizeB, int *nbE)
{
    if (!element)
	return;
    if(*nbE >= *sizeB){
	(*sizeB) *= 2;
	buffer = realloc(buffer, (*sizeB) * sizeof(void *));
    }
    buffer[(*nbE)++] = element;
}

void removeElementFromBuffer(void *buffer, int *nbE)
{
    *nbE > 0 ? (*nbE--) : (*nbE = 0);    
}
