#include "parser.h"
#include "testparser.h"

/* note that we do not need to separately test the
 * forwards and the turn functions for the parser,
 * because they are composed solely of an inc_word function
 * and a varnum function, both of which have been tested,
 * and the actual instruction (FD, LT, RT) has been tested
 * in test_instruction */

void test_all_functions(void) {

   test_inc_word();

   if(test_turtle_main()) {
      fprintf(stdout, "All turtle_main tests passed.\n");
   }
   else {
      fprintf(stderr, "At least one turtle_main test failed.\n");
      exit(1);
   }

   test_instruction_list();

   if(test_instruction()) {
      fprintf(stdout, "All instruction tests passed.\n");
   }
   else {
      fprintf(stderr, "At least one instruction test failed.\n");
      exit(1);
   }

   if(test_turtle_do()) {
      fprintf(stdout, "All turtle_do tests passed.\n");
   }
   else {
      fprintf(stderr, "At least one turtle_do test failed.\n");
      exit(1);
   }

   test_var();

   if(test_varnum()) {
      fprintf(stdout, "All varnum tests passed.\n");
   }
   else {
      fprintf(stderr, "At least one varnum test failed.\n");
      exit(1);
   }

   if(test_set()) {
      fprintf(stdout, "All set tests passed.\n");
   }
   else {
      fprintf(stderr, "At least one set test failed.\n");
      exit(1);
   }

   if(test_polish()) {
      fprintf(stdout, "All polish tests passed.\n");
   }
   else {
      fprintf(stderr, "At least one polish test failed.\n");
      exit(1);
   }

   test_op();

   fprintf(stdout, "All tests passed.\n");

}

void test_inc_word(void) {

   Program test;
   Program* tp;
   tp = &test;
   reset_words(tp);

   inc_word(tp);

   assert(tp->cword == 1);

   tp->cword = 734;

   inc_word(tp);

   assert(tp->cword == 735);

   tp->cword = 20;

   inc_word(tp);

   assert(tp->cword != 20);

   assert(tp->cword != 22);

   fprintf(stdout, "All inc_word tests passed.\n");

}

bool test_turtle_main(void) {

   Program test;
   Program* tp;
   tp = &test;
   reset_words(tp);
   tp->testing = 1;

   strcpy(tp->words[0], "Anything but a {.");
   turtle_main(tp);

   if(strsame(tp->error, "A { is required to start the program.\n")) {
      return true;
   }

   return false;
}

void test_instruction_list(void) {

   Program test;
   Program* tp;
   tp = &test;
   reset_words(tp);
   tp->testing = 1;

   strcpy(tp->words[0], "}");

   /* check program finishes when it should */
   assert(turtle_main(tp) == 1);

   fprintf(stdout, "All instruction_list tests passed.\n");

}

bool test_instruction(void) {

   Program test;
   Program* tp;
   tp = &test;
   tp->testing = 1;

   reset_words(tp);
   strcpy(tp->words[0], "FD");
   instruction(tp);

   if(strsame(tp->error, "The instruction has not been recognised.\n")) {
      return false;
   }

   reset_words(tp);
   strcpy(tp->words[0], "LT");
   instruction(tp);

   if(strsame(tp->error, "The instruction has not been recognised.\n")) {
      return false;
   }

   reset_words(tp);
   strcpy(tp->words[0], "RT");
   instruction(tp);

   if(strsame(tp->error, "The instruction has not been recognised.\n")) {
      return false;
   }

   reset_words(tp);
   strcpy(tp->words[0], "DO");
   instruction(tp);

   if(strsame(tp->error, "The instruction has not been recognised.\n")) {
      return false;
   }

   reset_words(tp);
   strcpy(tp->words[0], "SET");
   instruction(tp);

   if(strsame(tp->error, "The instruction has not been recognised.\n")) {
      return false;
   }

   reset_words(tp);
   strcpy(tp->words[0], "fd");
   instruction(tp);

   if(!strsame(tp->error, "The instruction has not been recognised.\n")) {
      return false;
   }

   reset_words(tp);
   strcpy(tp->words[0], "LTT");
   instruction(tp);

   if(!strsame(tp->error, "The instruction has not been recognised.\n")) {
      return false;
   }

   reset_words(tp);
   strcpy(tp->words[0], "banana078.2");
   instruction(tp);

   if(!strsame(tp->error, "The instruction has not been recognised.\n")) {
      return false;
   }

   reset_words(tp);
   strcpy(tp->words[0], "+78.2-1md.IO");
   instruction(tp);

   if(!strsame(tp->error, "The instruction has not been recognised.\n")) {
      return false;
   }

   return true;

}

bool test_turtle_do(void) {

   /* no need to test invocations of
    * inc_word, var varnum and instruction_list
    * as these have all been tested separately */

   /* have already checked that do is the
    * current word */

   Program test;
   Program* tp;
   tp = &test;
   tp->testing = 1;

   /* N.B. in interpreter, second varnum must be
    * greater than or equal to the first but this
    * doesn't matter in parser */

   reset_words(tp);
   strcpy(tp->words[1], "H");
   strcpy(tp->words[2], "FROM");
   strcpy(tp->words[3], "3");
   strcpy(tp->words[4], "TO");
   strcpy(tp->words[5], "1");
   strcpy(tp->words[6], "{");
   strcpy(tp->words[7], "}");

   assert(turtle_do(tp) == 1);

   /* N.B. in interpreter, varnums must be integers
    * or evaluate to integers but this doesn't
    * matter in parser */

   reset_words(tp);
   strcpy(tp->words[1], "M");
   strcpy(tp->words[2], "FROM");
   strcpy(tp->words[3], "1.93");
   strcpy(tp->words[4], "TO");
   strcpy(tp->words[5], "-1.2");
   strcpy(tp->words[6], "{");
   strcpy(tp->words[7], "}");

   assert(turtle_do(tp) == 1);

   reset_words(tp);
   strcpy(tp->words[1], "M");
   strcpy(tp->words[2], "from");
   strcpy(tp->words[3], "1.93");
   strcpy(tp->words[4], "TO");
   strcpy(tp->words[5], "-1.2");
   strcpy(tp->words[6], "{");
   strcpy(tp->words[7], "}");

   turtle_do(tp);

   if(!strsame(tp->error, "Expecting FROM in DO instruction.\n")) {
      return false;
   }

   reset_words(tp);
   strcpy(tp->words[1], "M");
   strcpy(tp->words[2], "FROM1");
   strcpy(tp->words[3], "1.93");
   strcpy(tp->words[4], "TO");
   strcpy(tp->words[5], "-1.2");
   strcpy(tp->words[6], "{");
   strcpy(tp->words[7], "}");

   turtle_do(tp);

   if(!strsame(tp->error, "Expecting FROM in DO instruction.\n")) {
      return false;
   }

   reset_words(tp);
   strcpy(tp->words[1], "M");
   strcpy(tp->words[2], "FROM");
   strcpy(tp->words[3], "1.93");
   strcpy(tp->words[4], "To");
   strcpy(tp->words[5], "-1.2");
   strcpy(tp->words[6], "{");
   strcpy(tp->words[7], "}");

   turtle_do(tp);

   if(!strsame(tp->error, "Expecting TO in DO instruction.\n")) {
      return false;
   }

   reset_words(tp);
   strcpy(tp->words[1], "M");
   strcpy(tp->words[2], "FROM");
   strcpy(tp->words[3], "1.93");
   strcpy(tp->words[4], "UNTIL");
   strcpy(tp->words[5], "-1.2");
   strcpy(tp->words[6], "{");
   strcpy(tp->words[7], "}");

   turtle_do(tp);

   if(!strsame(tp->error, "Expecting TO in DO instruction.\n")) {
      return false;
   }

   reset_words(tp);
   strcpy(tp->words[1], "M");
   strcpy(tp->words[2], "FROM");
   strcpy(tp->words[3], "1.93");
   strcpy(tp->words[4], "TO");
   strcpy(tp->words[5], "-1.2");
   strcpy(tp->words[6], "{{");
   strcpy(tp->words[7], "}");

   turtle_do(tp);

   if(!strsame(tp->error, "Expecting { in DO instruction.\n")) {
      return false;
   }

   reset_words(tp);
   strcpy(tp->words[1], "M");
   strcpy(tp->words[2], "FROM");
   strcpy(tp->words[3], "1.93");
   strcpy(tp->words[4], "TO");
   strcpy(tp->words[5], "-1.2");
   strcpy(tp->words[6], "}");
   strcpy(tp->words[7], "}");

   turtle_do(tp);

   if(!strsame(tp->error, "Expecting { in DO instruction.\n")) {
      return false;
   }

   return true;

}

void test_var(void) {

   Program test;
   Program* tp;
   tp = &test;
   reset_words(tp);

   strcpy(tp->words[0], "A");

   assert(var(tp) == true);

   strcpy(tp->words[0], "M");

   assert(var(tp) == true);

   strcpy(tp->words[0], "Z");

   assert(var(tp) == true);

   strcpy(tp->words[0], "5");

   assert(var(tp) == false);

   strcpy(tp->words[0], "3.78");

   assert(var(tp) == false);

   strcpy(tp->words[0], "ABC");

   assert(var(tp) == false);

   strcpy(tp->words[0], "q");

   assert(var(tp) == false);

   strcpy(tp->words[0], "_K");

   assert(var(tp) == false);

   strcpy(tp->words[0], "\0");

   assert(var(tp) == false);

   fprintf(stdout, "All var tests passed.\n");

}

bool test_varnum(void) {

   Program test;
   Program* tp;
   tp = &test;
   reset_words(tp);
   tp->testing = 1;

   strcpy(tp->words[0], "A");

   assert(varnum(tp) == true);

   strcpy(tp->words[0], "L");

   assert(varnum(tp) == true);

   strcpy(tp->words[0], "72");

   assert(varnum(tp) == true);

   strcpy(tp->words[0], "-34");

   assert(varnum(tp) == true);

   strcpy(tp->words[0], "96.31");

   assert(varnum(tp) == true);

   strcpy(tp->words[0], "-82.12");

   assert(varnum(tp) == true);

   strcpy(tp->words[0], "m");
   varnum(tp);

   if(!strsame(tp->error, "This is not a valid variable or number.\n")) {
      return false;
   }

   strcpy(tp->words[0], "hello");
   varnum(tp);

   if(!strsame(tp->error, "This is not a valid variable or number.\n")) {
      return false;
   }

   strcpy(tp->words[0], "-.123");
   varnum(tp);

   if(!strsame(tp->error, "This is not a valid variable or number.\n")) {
      return false;
   }

   strcpy(tp->words[0], "-130.12.9");
   varnum(tp);

   if(!strsame(tp->error, "This is not a valid variable or number.\n")) {
      return false;
   }

   strcpy(tp->words[0], "72.3.1");
   varnum(tp);

   if(!strsame(tp->error, "This is not a valid variable or number.\n")) {
      return false;
   }

   strcpy(tp->words[0], "72.3.1");
   varnum(tp);

   if(!strsame(tp->error, "This is not a valid variable or number.\n")) {
      return false;
   }

   strcpy(tp->words[0], "65.");
   varnum(tp);

   if(!strsame(tp->error, "This is not a valid variable or number.\n")) {
      return false;
   }

   strcpy(tp->words[0], "-29.");
   varnum(tp);

   if(!strsame(tp->error, "This is not a valid variable or number.\n")) {
      return false;
   }

   return true;

}

bool test_set(void) {

   /* no need to test invocations of
    * inc_word, var and varnum as these
    * have all been tested separately */

   /* have already checked that set is the
    * current word */

   Program test;
   Program* tp;
   tp = &test;
   tp->testing = 1;

   /* valid Polish for parsing but not for
    * interpreting */

   reset_words(tp);
   strcpy(tp->words[1], "H");
   strcpy(tp->words[2], ":=");
   strcpy(tp->words[3], "+");
   strcpy(tp->words[4], ";");

   assert(set(tp) == 1);

   /* valid Polish for parsing and
    * interpreting */

   reset_words(tp);
   strcpy(tp->words[1], "Z");
   strcpy(tp->words[2], ":=");
   strcpy(tp->words[3], "3");
   strcpy(tp->words[4], "2");
   strcpy(tp->words[5], "-");
   strcpy(tp->words[6], ";");

   assert(set(tp) == 1);

   reset_words(tp);
   strcpy(tp->words[1], "C");
   strcpy(tp->words[2], "=:");

   set(tp);

   if(!strsame(tp->error, "The assignment operator is missing or incorrect.\n")) {
      return false;
   }

   reset_words(tp);
   strcpy(tp->words[1], "J");
   strcpy(tp->words[2], "36.2");

   set(tp);

   if(!strsame(tp->error, "The assignment operator is missing or incorrect.\n")) {
      return false;
   }

   reset_words(tp);
   strcpy(tp->words[1], "U");
   strcpy(tp->words[2], "\0");

   set(tp);

   if(!strsame(tp->error, "The assignment operator is missing or incorrect.\n")) {
      return false;
   }

   reset_words(tp);
   strcpy(tp->words[1], "P");
   strcpy(tp->words[2], "sheep");

   set(tp);

   if(!strsame(tp->error, "The assignment operator is missing or incorrect.\n")) {
      return false;
   }

   return true;

}

bool test_polish(void) {

   Program test;
   Program* tp;
   tp = &test;
   tp->testing = 1;

   /* note the following tests are part of the
    * grammar and can be parsed successfully,
    * but cannot be interpreted */

   reset_words(tp);
   strcpy(tp->words[0], ";");

   assert(polish(tp) == 1);

   reset_words(tp);
   strcpy(tp->words[0], "+");
   strcpy(tp->words[1], ";");

   assert(polish(tp) == 1);

   reset_words(tp);
   strcpy(tp->words[0], "6");
   strcpy(tp->words[1], "-");
   strcpy(tp->words[2], ";");

   assert(polish(tp) == 1);

   reset_words(tp);
   strcpy(tp->words[0], "6");
   strcpy(tp->words[1], "0");
   strcpy(tp->words[2], "/");
   strcpy(tp->words[3], ";");

   assert(polish(tp) == 1);

   reset_words(tp);
   strcpy(tp->words[0], "A");
   strcpy(tp->words[1], "3");
   strcpy(tp->words[2], "+");
   strcpy(tp->words[3], "+");
   strcpy(tp->words[4], "6");
   strcpy(tp->words[5], ";");

   assert(polish(tp) == 1);


   /* the remaining tests can be interpreted
    * (provided the variables are set)
    * as well as parsed */

   reset_words(tp);
   strcpy(tp->words[0], "A");
   strcpy(tp->words[1], "3");
   strcpy(tp->words[2], "+");
   strcpy(tp->words[3], ";");

   assert(polish(tp) == 1);

   reset_words(tp);
   strcpy(tp->words[0], "A");
   strcpy(tp->words[1], "3.05");
   strcpy(tp->words[2], "+");
   strcpy(tp->words[3], "B");
   strcpy(tp->words[4], "-8.6");
   strcpy(tp->words[5], "*");
   strcpy(tp->words[6], "+");
   strcpy(tp->words[7], ";");

   assert(polish(tp) == 1);

   /* note this is legal polish because
    * the function returns 1 at the first
    * semicolon, however it would fail parsing because
    * ";" is not a valid instruction */

   reset_words(tp);
   strcpy(tp->words[0], "3");
   strcpy(tp->words[1], ";");
   strcpy(tp->words[2], ";");

   assert(polish(tp) == 1);

   reset_words(tp);
   strcpy(tp->words[0], "+");
   strcpy(tp->words[1], "3");

   polish(tp);

   if(!strsame(tp->error, "Not a valid operation, variable or number or no terminating semicolon.\n")) {
      return false;
   }

   reset_words(tp);
   strcpy(tp->words[0], "3");
   strcpy(tp->words[1], "cow");
   strcpy(tp->words[2], "+");
   strcpy(tp->words[3], ";");

   polish(tp);

   if(!strsame(tp->error, "Not a valid operation, variable or number or no terminating semicolon.\n")) {
      return false;
   }

   reset_words(tp);
   strcpy(tp->words[0], "3");
   strcpy(tp->words[1], "2");
   strcpy(tp->words[2], "=");
   strcpy(tp->words[3], ";");

   polish(tp);

   if(!strsame(tp->error, "Not a valid operation, variable or number or no terminating semicolon.\n")) {
      return false;
   }

   return true;

}

void test_op(void) {

   Program test;
   Program* tp;
   tp = &test;
   reset_words(tp);

   strcpy(tp->words[0], "+");

   assert(op(tp) == true);

   strcpy(tp->words[0], "-");

   assert(op(tp) == true);

   strcpy(tp->words[0], "*");

   assert(op(tp) == true);

   strcpy(tp->words[0], "/");

   assert(op(tp) == true);

   strcpy(tp->words[0], "A");

   assert(op(tp) == false);

   strcpy(tp->words[0], "\0");

   assert(op(tp) == false);

   strcpy(tp->words[0], "7983");

   assert(op(tp) == false);

   strcpy(tp->words[0], "++");

   assert(op(tp) == false);

   fprintf(stdout, "All op tests passed.\n");

}

void reset_words(Program *p) {

   int i;

   for(i = 0; i < MAXWORDS; i++) {
      strcpy(p->words[i], "\0");
   }

   p->cword = 0;
   strcpy(p->error, "\0");
}
