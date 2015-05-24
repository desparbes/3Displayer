#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct Texture {
    float x;
    float y;
} Texture;

float loopTexture(float a);
void setTexture(Texture *t, float x, float y);

#endif // TEXTURE_H
