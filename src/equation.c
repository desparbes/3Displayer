#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "equation.h"
#include "point.h"
#include "stack.h"
#include "state.h"

#define MAXLENGTH 256
#define NB_FUNCTIONS 4

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

static int analyseFunction(char *xyz, int *i)
{
    int j = 0;
    char *cos = "cos";
    char *sin = "sin";
    char *sqrt = "sqrt";
    char *sqr = "sqr";
    resetState(Equation.state);

    while(isLetter(xyz[*i])) {
	if (j < 3 && xyz[*i] != cos[j])
	    setRefuseState(Equation.state, COS);
	if (j < 3 && xyz[*i] != sin[j])
	    setRefuseState(Equation.state, SIN);
	if (j < 4 && xyz[*i] != sqrt[j])
	    setRefuseState(Equation.state, SQRT);
	if (j < 3 && xyz[*i] != sqr[j])
	    setRefuseState(Equation.state, SQR);
	j++;
	(*i)++;
    }
    return analyseState(Equation.state);
}

static int isBinaryOperator(char c)
{
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

int analyseBinaryOperator(char *xyz, int *i)
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
	    
static void getValueFromLine(char *xyz, float s, float t, float *XYZ)
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
	    }
	    addDataStack(Equation.stack, &elmtA);
	} else if (isBinaryOperator(xyz[i])) {
	    elmtA.value.operator = analyseBinaryOperator(xyz, &i);
	    elmtA.valueType = OPERATOR;
	    addDataStack(Equation.stack, &elmtA);
	} else if (xyz[i] == ')') {
	    elmtA = *((ElementEquation *)readDataStack(Equation.stack));
	    removeDataStack(Equation.stack);
	    if (elmtA.valueType == NUMBER) {
		elmtB = *((ElementEquation *)readDataStack(Equation.stack));
		removeDataStack(Equation.stack);
	        if (elmtB.valueType == OPERATOR) {
		    elmtC = *((ElementEquation *)readDataStack(Equation.stack));
		    removeDataStack(Equation.stack);
		    if (elmtC.valueType != NUMBER)
			fprintf(stderr, "Error parsing equation: bad + 1");
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
		} else
		    fprintf(stderr, "Error parsing equation\n");		
	    } else 
		fprintf(stderr, "Error parsing equation: structural problem\n");
	}
    }

    result = *((ElementEquation *)readDataStack(Equation.stack));
    removeDataStack(Equation.stack);
    
    if (result.valueType == NUMBER)
	*XYZ = result.value.number;
    else
	fprintf(stderr, "Error parsing equation: incomplete\n");
}

int initEquation(float *minS, float *maxS, int *precisionS,
		 float *minT, float *maxT, int *precisionT,
		 char *x, char *y, char *z, int stringSize, 
		 const char *eqName)
{
    FILE *file = fopen(eqName, "r");
    Equation.state = initState(NB_FUNCTIONS);
    Equation.stack = initStack(sizeof(ElementEquation));

    if (file == NULL) {
	perror(eqName);
        return 0;
    }
    char str[MAXLENGTH];
    
    while (fscanf(file, "%s", str) != EOF) {
	if (strcmp(str, "minS") == 0 && fscanf(file, "%f\n", minS) != 1) {
		fprintf(stderr, "Error loading minS\n");
		fclose(file);
		return 0;
	}
	if (strcmp(str, "maxS") == 0 && fscanf(file, "%f\n", maxS) != 1) {
		fprintf(stderr, "Error loading maxS\n");
		fclose(file);
		return 0;
	}
	if (strcmp(str, "precisionS") == 0 && 
	    fscanf(file, "%d\n", precisionS) != 1) {
		fprintf(stderr, "Error loading precisionS\n");
		fclose(file);
		return 0;
	}
	if (strcmp(str, "minT") == 0 && fscanf(file, "%f\n", minT) != 1) {
		fprintf(stderr, "Error loading minT\n");
		fclose(file);
		return 0;
	}
	if (strcmp(str, "maxT") == 0 && fscanf(file, "%f\n", maxT) != 1) {
		fprintf(stderr, "Error loading maxT\n");
		fclose(file);
		return 0;
	}
	if (strcmp(str, "precisionT") == 0 && 
	    fscanf(file, "%d\n", precisionT) != 1) {
		fprintf(stderr, "Error loading precisionT\n");
		fclose(file);
		return 0;
	}
	if (strcmp(str, "x") == 0 && fscanf(file, "%s\n", x) != 1) {
		fprintf(stderr, "Error loading x\n");
		fclose(file);
		return 0;
	}
	if (strcmp(str, "y") == 0 && fscanf(file, "%s\n", y) != 1) {
		fprintf(stderr, "Error loading y\n");
		fclose(file);
		return 0;
	}
	if (strcmp(str, "z") == 0 && fscanf(file, "%s\n", z) != 1) {
		fprintf(stderr, "Error loading z\n");
		fclose(file);
		return 0;
	}
    }
    fclose(file);
    return 1;
}

void getValueFromEquation(char *x, char *y, char *z, 
			  float s, float t, Point *p)
{
    getValueFromLine(x, s, t, &p->x);
    getValueFromLine(y, s, t, &p->y);
    getValueFromLine(z, s, t, &p->z);
}

void freeEquation()
{
    destroyStack(Equation.stack);
    destroyState(Equation.state);
}



