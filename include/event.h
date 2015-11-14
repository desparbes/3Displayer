#ifndef EVENT_H
#define EVENT_H

extern void (*initEvent)();
extern void (*handleEvent)(int *stop);
extern void (*freeEvent)();

#endif // EVENT_H
