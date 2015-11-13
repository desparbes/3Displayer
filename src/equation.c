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

static float analyseNumber(char *xyz, int *i)
{
    float number = 0.;
    float pow = 1.;
    int beginning = *i;
    int length = numberLength(&xyz[beginning]);
    int end = beginning + length;

    for (int j = end - 1; j >= beginning; j--) {
	number += pow * charToInt(xyz[j]);
	pow *= 10.;
    }

    if (xyz[end] == '.') {
	pow = 0.1;
	int j;
	for (j = end + 1; isNumber(xyz[j]); j++) {
	    number += pow * charToInt(xyz[j]);
	    pow /= 10.;
	}
	*i = j - 1;
    } else
	*i = end - 1;

    return number;
}

static inline int isLetter(char c)
{
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

static inline int isVariable(char *xyz, int i)
{
    return ((xyz[i] == 's' || xyz[i] == 't') && !isLetter(xyz[i + 1]));
}

static float analyseVariable(char c, float s, float t)
{
    if (c == 's')
	return s;
    else if (c == 't')
	return t;
    else 
	fprintf(stderr, "Error analyzing variable: doesn't exist\n");
    return 0.;
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
	    
static int getValueFromLine(char *xyz, float s, float t, float *XYZ)
{
    ElementEquation result;
    resetStack(Equation.stack);
    *XYZ = s + t;

    for (int i = 0; xyz[i]; i++) {
	ElementEquation elmtA;
	ElementEquation elmtB;
	ElementEquation elmtC;
	
	if (isNumber(xyz[i])) {
	    elmtA.value.number = analyseNumber(xyz, &i);
	    elmtA.valueType = NUMBER;
	    addDataStack(Equation.stack, &elmtA);
	} else if (isLetter(xyz[i])) {
	    if (isVariable(xyz, i)) {
		elmtA.value.number = analyseVariable(xyz[i], s, t);
		elmtA.valueType = NUMBER;
	    } else if ((elmtA.value.function = analyseFunction(xyz, &i)) != -1) {
		elmtA.valueType = FUNCTION;
	    } else {
		fprintf(stderr, "Error parsing equation: unknown symbol\n");
		return 0;
	    }
	    addDataStack(Equation.stack, &elmtA);
	} else if (isBinaryOperator(xyz[i])) {
	    elmtA.value.operator = analyseBinaryOperator(xyz, &i);
	    elmtA.valueType = OPERATOR;
	    addDataStack(Equation.stack, &elmtA);
	} else if (xyz[i] == ')' && !voidStack(Equation.stack)) {
	    elmtA = *((ElementEquation *)readDataStack(Equation.stack));
	    removeDataStack(Equation.stack);
	    if (elmtA.valueType == NUMBER && !voidStack(Equation.stack)) {
		elmtB = *((ElementEquation *)readDataStack(Equation.stack));
		removeDataStack(Equation.stack);
	        if (elmtB.valueType == OPERATOR && !voidStack(Equation.stack)) {
		    elmtC = *((ElementEquation *)readDataStack(Equation.stack));
		    removeDataStack(Equation.stack);
		    if (elmtC.valueType != NUMBER) {
			fprintf(stderr, "Error parsing equation: bad + 1");
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
		    fprintf(stderr, "Error parsing equation\n");		
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
    *XYZ = result.value.number;
    return 1;
}

static int initEquation(float *minS, float *maxS, int *precisionS,
		 float *minT, float *maxT, int *precisionT,
		 char *x, char *y, char *z, int stringSize, 
		 const char *eqName)
{
    FILE *file = fopen(eqName, "r");
    if (file == NULL) {
	perror(eqName);
        return 0;
    }
    
    char str[MAXLENGTH];
    int check[NB_KEYWORDS] = {0};
    int template[NB_KEYWORDS];
    initArray(template, NB_KEYWORDS, 1);
    
    while (fscanf(file, "%s", str) != EOF) {
	if (strcmp(str, "minS") == 0 && fscanf(file, "%f", minS) == 1)
	    check[MINS]++;
	else if (strcmp(str, "maxS") == 0 && fscanf(file, "%f", maxS) == 1)
	    check[MAXS]++;
	else if (strcmp(str, "precisionS") == 0 &&
	    fscanf(file, "%d", precisionS) == 1)
	    check[PRECISIONS]++;
	else if (strcmp(str, "minT") == 0 && fscanf(file, "%f", minT) == 1)
	    check[MINT]++;
	else if (strcmp(str, "maxT") == 0 && fscanf(file, "%f", maxT) == 1)
	    check[MAXT]++;
	else if (strcmp(str, "precisionT") == 0 && 
	    fscanf(file, "%d", precisionT) == 1)
	    check[PRECISIONT]++;
	else if (strcmp(str, "x") == 0 && fscanf(file, "%s", x) == 1)
	    check[X]++;
	else if (strcmp(str, "y") == 0 && fscanf(file, "%s", y) == 1)
	    check[Y]++;
	else if (strcmp(str, "z") == 0 && fscanf(file, "%s", z) == 1)
	    check[Z]++;
    }
    if (!areEqualsArray(check, template, NB_KEYWORDS)) {
	printf("Error parsing equation\n");
	return 0;
    }

    Equation.state = initState(NB_FUNCTIONS);
    Equation.stack = initStack(sizeof(ElementEquation));
    fclose(file);
    return 1;
}

static int getValueFromEquation(char *x, char *y, char *z, 
			 float s, float t, Point *p)
{
    return (getValueFromLine(x, s, t, &p->x) &&
	    getValueFromLine(y, s, t, &p->y) &&
	    getValueFromLine(z, s, t, &p->z));
}

static void freeEquation()
{
    destroyStack(Equation.stack);
    destroyState(Equation.state);
}

Solid *loadEquation(const char *eqName, const char *bmpName)
{
    float minS, maxS, minT, maxT;
    int precisionS, precisionT;
    char x[MAXLENGTH];
    char y[MAXLENGTH];
    char z[MAXLENGTH];
    
    if (!initEquation(&minS, &maxS, &precisionS,
		      &minT, &maxT, &precisionT,
		      x, y, z, MAXLENGTH, eqName)) {
	fprintf(stderr, "Error loading equation\n");
	return NULL;
    }

    Solid *solid = malloc(sizeof(Solid));
    int p, f = 0;
    float s = minS, t = minT;
    float ds = (maxS - minS) / (precisionS - 1);
    float dt = (maxT - minT) / (precisionT - 1);

    solid->numVertices = precisionS * precisionT;
    solid->numNormals = solid->numVertices;
    solid->numCoords = 4;
    solid->numFaces = 2 * (precisionS - 1) * (precisionT - 1);

    solid->vertices = malloc(solid->numVertices * sizeof(Point));
    solid->normals = malloc(solid->numNormals * sizeof(Point));
    solid->coords = malloc(solid->numCoords * sizeof(Position));
    solid->faces = malloc(solid->numFaces * sizeof(Face));

    setPosition(&solid->coords[0], 0., 0.);
    setPosition(&solid->coords[1], 0., 1.);
    setPosition(&solid->coords[2], 1., 0.);
    setPosition(&solid->coords[3], 1., 1.);

    if ((solid->texture = loadTexture(bmpName)))
	printf("Texture successfully loaded\n");
    else
	printf("Error loading texture\n");

    if (solid->numVertices > 0) {
	HANDLE(getValueFromEquation(x, y, z, s, t, &solid->vertices[0]))
    }

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

	    s = minS;
	    t += dt;
	} else if (p >= (solid->numVertices - precisionS)) { // north
	    A = &solid->vertices[p - precisionS];
	    B = &solid->vertices[p + 1];
	    
	    s += ds;
	} else if (p < precisionS) { // south
	    A = &solid->vertices[p + 1];
	    B = &solid->vertices[p + precisionS];
	    
	    HANDLE(getValueFromEquation(x, y, z, s + ds, t, A))
	    HANDLE(getValueFromEquation(x, y, z, s, t + dt, B))
	    s += ds;
	} else { // elsewhere
	    A = &solid->vertices[p + 1];
	    B = &solid->vertices[p + precisionS];
	    
	    HANDLE(getValueFromEquation(x, y, z, s, t + dt, B))
	    s += ds;
	}
	diffPoint(A, O, &u);
	diffPoint(B, O, &v);
	pointProduct(&u, &v, normal);
	normalizePoint(normal, normal);
    }

    p = 0;
    while (f < solid->numFaces) {
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
	p++;
    }
    freeEquation();
    printf("Equation successfully loaded\n");
    return solid;
}



