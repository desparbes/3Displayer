#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct Texture {
    float x;
    float y;
} Texture;

void loopTexture(Texture *t);
void setTexture(Texture *t, float x, float y);

#endif // TEXTURE_H
