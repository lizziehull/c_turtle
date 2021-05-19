#ifndef TURTLE_EXTENSION_H
#define TURTLE_EXTENSION_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include "neillsdl2.h"

#define MAXWORDS 500 /* can be increased to run a longer program */
#define MAXCMDSIZE 30
/* can be increased in the unlikely case a varnum string
 * (i.e. a number string because the grammar only allows
 * single-letter variables needs to exceed this length */
#define strsame(A, B) (strcmp(A, B) == 0)
#define CWORD p->words[p->cword]
/* often used so abbreviated
 * CWORD stands for current word and keeps track
 * of what word of the grammar is being parsed */
#define PI 3.14159265358979323846
#define MILLISECONDDELAY 20
#define SWITCH 1 /* switches forward direction - right is 1, left is -1 */
#define SETLB -129847923.0983
/* some large number that the user is extremely unlikely to choose
 * otherwise complains there is a possibility that lowerbound is unset */
#define DEBUG 0 /* select 1 for debugging prints */
#define USINGSDL 1 /* select 0 to turn off SDL (e.g. to check leaks) */
#define SCALE 2 /* multiplies the distance of a FD command by x */
#define DASHGAP 2 /* higher number = more space between dashes - has to be int */

enum bool {false, true};
typedef enum bool bool;

struct position{
    bool pen; /* true for pen down, false for pen up */
    int dash;
    SDL_Simplewin win;
    int colour_set;
    double angle;
    double x;
    double dash_x;
    double y;
    double dash_y;
};

typedef struct position Position;

struct elem {
    char c[MAXCMDSIZE + 2];
    struct elem *prev;
};

typedef struct elem Elem;

struct stack {
    Elem *tp;
};

typedef struct stack Stack;

struct vartable{
    char c;
    double d;
};

typedef struct vartable Vartable;

struct varhelper{
    int index;
    int top;
    double value;
};

typedef struct varhelper Varhelper;

struct prog{
    char words[MAXWORDS][MAXCMDSIZE];
    int cword; /* current word */
    Position pos;
    Stack stk;
    Vartable vartbl[MAXWORDS];
    Varhelper vhelper;
};


typedef struct prog Program;

int inc_word(Program *p);
int turtle_main(Program *p);
int instruction_list(Program *p);
int instruction(Program *p);
int forwards(Program *p);
int turn(Program *p);
int turtle_do(Program *p);
bool var(Program *p, char* mode, char* word);
bool varnum(Program *p);
int set(Program *p);
int polish(Program *p);
bool op(Program *p);

bool is_int(double num);

int select_colour(Program *p);
int penup(Program *p);
int pendown(Program *p);
int dashline(Program *p);
int conditional(Program *p);
bool conditional_op(Program *p);

int initialise_stack(Stack *s);
int push(Stack *s, char n[MAXCMDSIZE]);
int pop(Stack *s, char rd[MAXCMDSIZE]);


#endif
