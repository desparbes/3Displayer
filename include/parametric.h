#ifndef PARAMETRIC_H
#define PARAMETRIC_H

int initEquation(float **min, float **max, int **precision,
                 int *nbInput, int *nbOutput, const char *eqName);
void getValueFromEquation(float *input, float *output);
void freeEquation();

#endif // PARAMETRIC_H
