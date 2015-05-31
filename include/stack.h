#ifndef STACK_H
#define STACK_H

typedef struct Stack Stack;

Stack *initStack(int dataSize);
void resetStack(Stack *stack);
int voidStack(Stack *stack);
void addDataStack(Stack *stack, void *data);
void *readDataStack(Stack *stack);
void removeDataStack(Stack *stack);
void destroyStack(Stack *stack);

#endif // STACK_H
