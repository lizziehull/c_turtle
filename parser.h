#ifndef TURTLE_PARSER_H
#define TURTLE_PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#define MAXWORDS 500 /* can be increased to run a longer program */
#define MAXCMDSIZE 30
#define ERRORSIZE 150
/* can be increased in the unlikely case a varnum string
 * (i.e. a number string because the grammar only allows
 * single-letter variables needs to exceed this length */
#define strsame(A, B) (strcmp(A, B) == 0)
#define CWORD p->words[p->cword]
/* often used so abbreviated
 * CWORD stands for current word and keeps track
 * of what word of the grammar is being parsed */
#define DEBUG 0 /* select 1 for debugging prints */
#define TESTING 0 /* select 1 to turn on testing */

struct prog{
    char words[MAXWORDS][MAXCMDSIZE];
    int cword; /* word counter - used to index char** words */
    char error[ERRORSIZE]; /* used to check error messages when testing */
    int testing;
};

typedef struct prog Program;

enum bool {false, true};
typedef enum bool bool;

void my_error(char err[ERRORSIZE], Program *p);

int inc_word(Program *p);
int turtle_main(Program *p);
int instruction_list(Program *p);
int instruction(Program *p);
int forwards(Program *p);
int turn(Program *p);
int turtle_do(Program *p);
bool var(Program *p);
bool varnum(Program *p);
int set(Program *p);
int polish(Program *p);
bool op(Program *p);

#endif
