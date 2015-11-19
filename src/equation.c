#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "build.h"
#include "position.h"
#include "texture.h"
#include "point.h"
#include "stack.h"
#include "state.h"
#include "array.h"
#include "equation.h"

#define MAXLENGTH 256
#define NB_FUNCTIONS 4
#define NB_KEYWORDS 9


#define HANDLE(x) if(!x){printf("Equation error\n");freeEquation();return NULL;}

typedef struct ElementEquation {
    int valueType;
    union {
	int function;
	int operator;
	float number;
    } value;
} ElementEquation;

static struct{
    State *state;
    Stack *stack;
    int input;
    int output;
    char **equation;
} Equation;

enum {
    FUNCTION, OPERATOR, NUMBER
};

enum {
    COS, SIN, SQRT, SQR
};

enum {
    PLUS, MINUS, TIMES, OVER
};

enum {
    POTENTIAL, ACCEPT, REFUSE
};

enum {
    MINS, MAXS, PRECISIONS, MINT, MAXT, PRECISIONT, X, Y, Z
};

static inline int isNumber(char c)
{
    return (c >= '0' && c <= '9');
}

static inline int charToInt(char c)
{
    return c - '0';
}

static int numberLength(char *number)
{
    int i;
    for (i = 0; isNumber(number[i]); i++);
    return i;
}

static int analyseInt(char *xyz, int *i)
{
    int number = 0;
    int pow = 1.;
    int beginning = *i;
    int length = numberLength(&xyz[beginning]);
    int end = beginning + length;

    for (int j = end - 1; j >= beginning; j--) {
	number += pow * charToInt(xyz[j]);
	pow *= 10.;
    }
    *i = end - 1;
    return number;
}

static float analyseFloat(char *xyz, int *i)
{
    float number = analyseInt(xyz, i);

    if (xyz[*i + 1] == '.') {
	float pow = 0.1;
	int j;
	for (j = *i + 2; isNumber(xyz[j]); j++) {
	    number += pow * charToInt(xyz[j]);
	    pow /= 10.;
	}
	*i = j - 1;
    }

    return number;
}


static inline int isLetter(char c)
{
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

static float analyseVariable(char *s, int *i, float *input)
{
    *i = *i + 1;
    int variable = analyseInt(s, i);

    if (variable < 0 || variable >= Equation.input) {
	fprintf(stderr, "Error analyzing variable: index not in range\n");
	return 0;
    }
    return input[variable];
}

static void handleFunction(int id, char xyz, char f, int fFinalCharId, int j)
{
    if ((getState(Equation.state, id) == POTENTIAL && 
	 j < fFinalCharId && xyz != f) ||
	(getState(Equation.state, id) == ACCEPT && j > fFinalCharId))
	setState(Equation.state, REFUSE, id);
    if (getState(Equation.state, id) == POTENTIAL && 
	j == fFinalCharId && xyz == f)
	setState(Equation.state, ACCEPT, id);
}


static int analyseFunction(char *xyz, int *i)
{
    int j = 0;
    char *cos = "cos";
    char *sin = "sin";
    char *sqrt = "sqrt";
    char *sqr = "sqr";
    resetState(Equation.state, POTENTIAL);
   
    while(isLetter(xyz[*i])) {
	handleFunction(COS, xyz[*i], cos[j], 2, j);
	handleFunction(SIN, xyz[*i], sin[j], 2, j);
	handleFunction(SQRT, xyz[*i], sqrt[j], 3, j);
	handleFunction(SQR, xyz[*i], sqr[j], 2, j);
	j++;
	(*i)++;
    }
    return analyseState(Equation.state, ACCEPT);
}

static int isBinaryOperator(char c)
{
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

static int analyseBinaryOperator(char *xyz, int *i)
{
    switch (xyz[*i]) {
    case '+':
	return PLUS;
	break;
    case '-':
	return MINUS;
	break;
    case '*':
	return TIMES;
	break;
    case '/':
	return OVER;
	break;
    default:
	return -1;
	break;
    }
    return -1;
}

static float computeOperator(float x, int operator, float y)
{
    switch (operator) {
    case PLUS:
	return x + y;
	break;
    case MINUS:
	return x - y;
	break;
    case TIMES:
	return x * y;
	break;
    case OVER:
	if (y == 0.) {
	    fprintf(stderr, "Error parsing equation: division by zero\n");
	    return 0.;
	}
	return x / y;
	break;
    default:
	fprintf(stderr, "Error parsing equation: unknown operator\n");
	return 0.;
	break;
    }
    return 0.;
}

static float computeFunction(int function, float x)
{
    switch (function) {
    case COS:
	return cos(x);
	break;
    case SIN:
	return sin(x);
	break;
    case SQRT:
	return sqrt(x);
	break;
    case SQR:
	return x * x;
	break;
    default:
	fprintf(stderr, "Error parsing equation: unknown function\n");
	return 0.;
	break;
    }
    return 0.;
}	
	    
static float getValue(float *input, int output)
{
    ElementEquation result;
    resetStack(Equation.stack);
    char *s = Equation.equation[output];

    for (int i = 0; s[i]; i++) {
	ElementEquation elmtA;
	ElementEquation elmtB;
	ElementEquation elmtC;
	
	if (isNumber(s[i])) {
	    elmtA.value.number = analyseFloat(s, &i);
	    elmtA.valueType = NUMBER;
	    addDataStack(Equation.stack, &elmtA);
	} else if (isLetter(s[i])) {
	    if ((elmtA.value.function = analyseFunction(s, &i)) != -1) {
		elmtA.valueType = FUNCTION;
	    } else {
		fprintf(stderr, "Error parsing equation: unknown symbol\n");
		return 0;
	    }
	    addDataStack(Equation.stack, &elmtA);
	} else if (s[i] == '$') {
	    elmtA.value.number = analyseVariable(s, &i, input);
	    elmtA.valueType = NUMBER;
	    addDataStack(Equation.stack, &elmtA);
	} else if (isBinaryOperator(s[i])) {
	    elmtA.value.operator = analyseBinaryOperator(s, &i);
	    elmtA.valueType = OPERATOR;
	    addDataStack(Equation.stack, &elmtA);
	} else if (s[i] == ')') {
	    if (voidStack(Equation.stack)) {
		fprintf(stderr, "Error parsing equation: expected expression before ')'\n");
		return 0;
	    }			
		
	    elmtA = *((ElementEquation *)readDataStack(Equation.stack));
	    removeDataStack(Equation.stack);
	    if (elmtA.valueType == NUMBER) {
		if (voidStack(Equation.stack)) {
		fprintf(stderr, "Error parsing equation: expected expression before number\n");
		return 0;
	    }
		elmtB = *((ElementEquation *)readDataStack(Equation.stack));
		removeDataStack(Equation.stack);
	        if (elmtB.valueType == OPERATOR) {
		    if (voidStack(Equation.stack)) {
			fprintf(stderr, "Error parsing equation: expecting expression before operator\n");
			return 0;
		    }			
			
		    elmtC = *((ElementEquation *)readDataStack(Equation.stack));
		    removeDataStack(Equation.stack);
		    if (elmtC.valueType != NUMBER) {
			fprintf(stderr, "Error parsing equation: first argument of operator is not a number\n");
			return 0;
		    }			
		    result.value.number = computeOperator(elmtC.value.number, 
							  elmtB.value.operator, 
							  elmtA.value.number);
		    result.valueType = NUMBER;
		    addDataStack(Equation.stack, &result);
		} else if (elmtB.valueType == FUNCTION) {
		    result.value.number = computeFunction(elmtB.value.function, 
							  elmtA.value.number);
		    result.valueType = NUMBER;
		    addDataStack(Equation.stack, &result);
		} else {
		    fprintf(stderr, "Error parsing equation: double number\n");
		    return 0;
		}
	    } else {
		fprintf(stderr, "Error parsing equation: structural problem\n");
		return 0;
	    }
	}
    }

    if (voidStack(Equation.stack)) {
	fprintf(stderr, "Error parsing equation: no result\n");
	return 0;
    }

    result = *((ElementEquation *)readDataStack(Equation.stack));
    removeDataStack(Equation.stack);

    if (result.valueType != NUMBER) {
	fprintf(stderr, "Error parsing equation: not a number\n");
	return 0;
    }
    return result.value.number;
}

static void freeEquation()
{
    destroyStack(Equation.stack);
    destroyState(Equation.state);
    for (int i = 0; i < Equation.output; i++)
	free(Equation.equation[i]);
    free(Equation.equation);
}

static int initEquation(float **min, float **max, int **precision, 
			int *nbInput, int *nbOutput, const char *eqName) 
{
    FILE *file = fopen(eqName, "r");
    if (file == NULL) {
	perror(eqName);
        return 0;
    }
    
    char str[MAXLENGTH];
    int i = 0, j = 0;
    Equation.input = 0;
    Equation.output = 0;

    while (fscanf(file, "%s", str) != EOF) {
	if (strcmp(str, "input") == 0)
	    Equation.input++;
	else if (strcmp(str, "output") == 0)
	    Equation.output++;
    }
    rewind(file);

    *min = malloc(Equation.input * sizeof(float));
    *max = malloc(Equation.input * sizeof(float));
    *precision = malloc(Equation.input * sizeof(int));
    Equation.equation = malloc(Equation.output * sizeof(char *));

    while (fscanf(file, "%s", str) != EOF) {
	if (strcmp(str, "input") == 0 && 
	    fscanf(file, "%f %f %d", &((*min)[i]), &((*max)[i]), &((*precision)[i])))
	    i++;
	else if (strcmp(str, "output") == 0 && fscanf(file, "%s", str)) {
	    Equation.equation[j] = malloc((strlen(str) + 1) * sizeof(char));
	    strcpy(Equation.equation[j], str);
	    j++;
	}
    }

    Equation.state = initState(NB_FUNCTIONS);
    Equation.stack = initStack(sizeof(ElementEquation));
    *nbInput = Equation.input;
    *nbOutput = Equation.output;

    fclose(file);
    if (i != Equation.input || j != Equation.output) {
	freeEquation();
	return 0;
    }
    return 1;
}

static void getValueFromEquation(float *input, float *output)
{
    for (int i = 0; i < Equation.output; i++)
	output[i] = getValue(input, i);
}



static int minimum(int a, int b)
{
    return a < b ? a : b;
}

static int getNumVertices(int *precision, int size)
{
    int r = 1;
    for (int i = 0; i < size; i++) {
	r *= precision[i];
    }
    return r;
}

static int getNumSegments(int *precision, int size)
{
    int s = 0;
    for (int i = 0; i < size; i++) {
	int p = (precision[i] - 1);
	for (int j = 1; j < size; j++)
	    p *= precision[(i + j) % size];
	s += p;
    }
    return s;
}

static void getGridFromId(int id, int size, int *precision, int *grid, int dim)
{
    for (int i = dim - 1; i >= 0; i--) {
	size /= precision[i];
	grid[i] = id / size;
	id -= grid[i] * size;
    }
}

static void updateInput(float *input, float *min, float *interval, 
			int *grid, int dim)
{
    for (int i = 0; i < dim; i++)
	input[i] = min[i] + interval[i] * grid[i];
}

static void mapOutput(float *output, Point *A, int dim)
{
    switch (dim) {
    case 1:
	A->x = output[0];
	A->y = 0;
	A->z = 0;
	break;
    case 2:
	A->x = output[0];
	A->y = output[1];
	A->z = 0;
	break;
    case 3:
	A->x = output[0];
	A->y = output[1];
	A->z = output[2];
	break;
    default:
	A->x = 0;
	A->y = 0;
	A->z = 0;
	break;
    }
}

Solid *loadEquation(const char *eqName, const char *bmpName)
{
    float *min = NULL, *max = NULL;
    int *precision = NULL;
    int nbInput;
    int nbOutput;
    
    if (!initEquation(&min, &max, &precision, &nbInput, &nbOutput, eqName)) {
	fprintf(stderr, "Error loading equation\n");
	return NULL;
    }

    Solid *solid = malloc(sizeof(Solid));
    float *input = malloc(nbInput * sizeof(float));
    float *output = malloc(nbOutput * sizeof(float));
    float *interval = malloc(nbInput * sizeof(float));
    int minInput = minimum(nbInput, 3);
    int p, e = 0;
    for (int i = 0; i < nbInput; i++) {
	input[i] = min[i];
	interval[i] = (max[i] - min[i]) / (precision[i] - 1);
    } 

    solid->numVertices = getNumVertices(precision, minInput);
    solid->numSegments = getNumSegments(precision, minInput);
    solid->numCoords = 4;    

    if (nbInput == 2) {
	solid->numNormals = solid->numVertices;
	solid->numFaces = 2 * (precision[0] - 1) * (precision[1] - 1);

	if ((solid->texture = loadTexture(bmpName)))
	    printf("Texture successfully loaded\n");
	else
	    printf("Error loading texture\n");

    } else {
	solid->numNormals = 0;
	solid->numFaces = 0;
	solid->texture = NULL;
    }

    solid->vertices = malloc(solid->numVertices * sizeof(Point));
    solid->normals = malloc(solid->numNormals * sizeof(Point));
    solid->segments = malloc(solid->numSegments * sizeof(Segment));
    solid->faces = malloc(solid->numFaces * sizeof(Face));
    solid->coords = malloc(solid->numCoords * sizeof(Position));

    setPosition(&solid->coords[0], 0., 0.);
    setPosition(&solid->coords[1], 0., 1.);
    setPosition(&solid->coords[2], 1., 0.);
    setPosition(&solid->coords[3], 1., 1.);

    for (p = 0; p < solid->numVertices; p++) {
	int grid[3];

	getGridFromId(p, solid->numVertices, precision, grid, minInput);
	updateInput(input, min, interval, grid, minInput);
	getValueFromEquation(input, output);
	mapOutput(output, &solid->vertices[p], nbOutput);
	
	int delta = 1;
	for (int i = 0; i < minInput; i++) {
	    if (grid[i] < precision[i] - 1) {
	        solid->segments[e].A = p; 
		solid->segments[e].B = p + delta; 
		e++;
	    }
	    delta *= precision[i];
	}
	
	if (nbInput == 2) {
	    

	}
	
    }

    /*
    for (p = 0; p < solid->numVertices; p++) {
	Point *A;
	Point *B;
	Point *O = &solid->vertices[p];
	Point *normal = &solid->normals[p];
	Point u;
	Point v;

        if (p == solid->numVertices - 1) { // north-east
	    A = &solid->vertices[p - 1];
	    B = &solid->vertices[p - precisionS];
	} else if (p % precisionS == precisionS - 1) { // east
	    A = &solid->vertices[p + precisionS];
	    B = &solid->vertices[p - 1];

	    HANDLE(getValueFromEquation(x, y, z, s, t + dt, A))
	    solid->segments[e].A = p; 
	    solid->segments[e].B = p + precisionS; 	    
	    e++;

	    s = minS;
	    t += dt;
	} else if (p >= (solid->numVertices - precisionS)) { // north
	    A = &solid->vertices[p - precisionS];
	    B = &solid->vertices[p + 1];

	    solid->segments[e].A = p; 
	    solid->segments[e].B = p + 1;	    
	    e++;

	    s += ds;
	} else { //elsewhere
	    A = &solid->vertices[p + 1];
	    B = &solid->vertices[p + precisionS];
	    
	    if (p < precisionS) // south
		HANDLE(getValueFromEquation(x, y, z, s + ds, t, A))
	    HANDLE(getValueFromEquation(x, y, z, s, t + dt, B))
	    solid->segments[e].A = p; 
	    solid->segments[e].B = p + 1; 
	    e++;
	    solid->segments[e].A = p; 
	    solid->segments[e].B = p + precisionS; 
	    e++;
	    solid->segments[e].A = p; 
	    solid->segments[e].B = p + precisionS + 1; 
	    e++;

	    s += ds;
	}
	diffPoint(A, O, &u);
	diffPoint(B, O, &v);
	pointProduct(&u, &v, normal);
	normalizePoint(normal, normal);
    }

    for (p = 0; f < solid->numFaces; p++) {
	if (p % precisionS != precisionS - 1) {
	    solid->faces[f].vertices[0].point = p;
	    solid->faces[f].vertices[1].point = p + 1;
	    solid->faces[f].vertices[2].point = p + precisionS;
	    solid->faces[f].vertices[0].normal = p;
	    solid->faces[f].vertices[1].normal = p + 1;
	    solid->faces[f].vertices[2].normal = p + precisionS;
	    solid->faces[f].vertices[0].coord = 1;
	    solid->faces[f].vertices[1].coord = 3;
	    solid->faces[f].vertices[2].coord = 0;

	    f++;

	    solid->faces[f].vertices[0].point = p + precisionS;
	    solid->faces[f].vertices[1].point = p + 1;
	    solid->faces[f].vertices[2].point = p + 1 + precisionS;
	    solid->faces[f].vertices[0].normal = p + precisionS;
	    solid->faces[f].vertices[1].normal = p + 1;
	    solid->faces[f].vertices[2].normal = p + 1 + precisionS;
	    solid->faces[f].vertices[0].coord = 0;
	    solid->faces[f].vertices[1].coord = 3;
	    solid->faces[f].vertices[2].coord = 2;

	    f++;
	}
    }
    */
    free(min);
    free(max);
    free(precision);
    free(interval);
    free(input);
    free(output);
    freeEquation();
    printf("Equation successfully loaded\n");
    return solid;
}



