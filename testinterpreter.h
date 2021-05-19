#ifndef TURTLE_TESTINTERPRETER_H
#define TURTLE_TESTINTERPRETER_H

#define TESTWORD tp->words[tp->cword]
#define BUFFER 0.0001 /* to account for rounding errors when using doubles */

void test_all_functions(void);

void test_inc_word(void);

bool test_turtle_main(void);
bool test_instruction(void);
bool test_forwards(void);
bool test_turn(void);
bool test_turtle_do(void);
bool test_var(void);
bool test_varnum(void);
bool test_set(void);
void test_op(void);
void test_is_int(void);

void reset_words(Program *p);

void reset_vartable(Program *p);

bool doubles_equal_buffer(double num1, double num2);

#endif
