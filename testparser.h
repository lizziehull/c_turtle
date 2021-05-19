#ifndef TURTLE_TESTPARSER_H
#define TURTLE_TESTPARSER_H

void test_all_functions(void);

void test_inc_word(void);

bool test_turtle_main(void);
void test_instruction_list(void);
bool test_instruction(void);
bool test_turtle_do(void);
void test_var(void);
bool test_varnum(void);
bool test_set(void);
bool test_polish(void);
void test_op(void);


void reset_words(Program *p);


#endif
