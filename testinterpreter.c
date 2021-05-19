#include "interpreter.h"
#include "testinterpreter.h"

void test_all_functions(void) {

   test_inc_word();

   if(test_turtle_main()) {
      fprintf(stdout, "All turtle_main tests passed.\n");
   }
   else {
      fprintf(stderr, "At least one turtle_main test failed.\n");
      exit(1);
   }

   if(test_instruction()) {
      fprintf(stdout, "All instruction tests passed.\n");
   }
   else {
      fprintf(stderr, "At least one instruction test failed.\n");
      exit(1);
   }

   if(test_forwards()) {
      fprintf(stdout, "All forwards tests passed.\n");
   }
   else {
      fprintf(stderr, "At least one forwards test failed.\n");
      exit(1);
   }

   if(test_turn()) {
      fprintf(stdout, "All turn tests passed.\n");
   }
   else {
      fprintf(stderr, "At least one turn test failed.\n");
      exit(1);
   }

   if(test_turtle_do()) {
      fprintf(stdout, "All turtle_do tests passed.\n");
   }
   else {
      fprintf(stderr, "At least one turtle_do test failed.\n");
      exit(1);
   }

   if(test_var()) {
      fprintf(stdout, "All var tests passed.\n");
   }
   else {
      fprintf(stderr, "At least var test failed.\n");
      exit(1);
   }

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

   test_op();

   test_is_int();

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

bool test_forwards(void) {

   /* already checked that the current
    * word is FD */

   /* starting coordinates are
    x = 400, y = 300 */

   /* we are assuming SWITCH is
    * set to 1 */

   Program test;
   Program* tp;
   double testx, testy;
   tp = &test;
   tp->testing = 1;

   reset_words(tp);
   reset_vartable(tp);

   tp->pos.angle = 0.0;

   strcpy(tp->words[1], "50");

   forwards(tp);

   if(!doubles_equal_buffer(tp->pos.x, 450.00) || !doubles_equal_buffer(tp->pos.y, 300.0)) {
      return false;
   }

   /* going backwards */

   tp->pos.angle = PI;

   reset_words(tp);

   strcpy(tp->words[1], "100");

   forwards(tp);

   if(!doubles_equal_buffer(tp->pos.x, 300.0) || !doubles_equal_buffer(tp->pos.y, 300.0)) {
      return false;
   }

   /* going in a random direction */

   tp->pos.angle = 3;

   reset_words(tp);

   strcpy(tp->words[1], "150");

   testx = 400.0 + cos(3) * 150.0;
   testy = 300.0 + sin(3) * 150.0;

   if(!doubles_equal_buffer(tp->pos.x, testx) || !doubles_equal_buffer(tp->pos.y, testy)) {
      return false;
   }

   return true;

}

bool test_turn(void) {

   /* already checked that the current
    * word is LT or RT */

    /* Note that the user enters LT <VARNUM>
     * or RT <VARNUM> but there is actually
     * only one turn function that reverses
     * the direction of the turn if it is a
     * RT (a negative number passed
     * into LT or RT also reverses the direction
     * of the turn). Angles are entered in degrees
     * and converted to radians. Here for ease
     * of use, I convert the result back to degrees
     * using (180 / PI). A value of greater than
     * 360 or less than -360 is allowed as sin
     * and cos are periodic */

   Program test;
   Program* tp;
   tp = &test;
   tp->testing = 1;

   reset_words(tp);
   reset_vartable(tp);

   tp->pos.angle = 0.0;

   strcpy(tp->words[0], "RT");
   strcpy(tp->words[1], "50");

   turn(tp);

   if(!doubles_equal_buffer(tp->pos.angle * (180 / PI), -50.0)) {
      return false;
   }

   reset_words(tp);

   strcpy(tp->words[0], "LT");
   strcpy(tp->words[1], "65.5");

   turn(tp);

   if(!doubles_equal_buffer(tp->pos.angle * (180 / PI), 15.5)) {
      return false;
   }

   reset_words(tp);

   strcpy(tp->words[0], "LT");
   strcpy(tp->words[1], "-15.5");

   turn(tp);

   if(!doubles_equal_buffer(tp->pos.angle * (180 / PI), 0.0)) {
      return false;
   }

   reset_words(tp);

   strcpy(tp->words[0], "RT");
   strcpy(tp->words[1], "-480");

   turn(tp);

   if(!doubles_equal_buffer(tp->pos.angle * (180 / PI), 480.0)) {
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
   reset_vartable(tp);
   strcpy(tp->words[1], "H");
   strcpy(tp->words[2], "FROM");
   strcpy(tp->words[3], "3");
   strcpy(tp->words[4], "TO");
   strcpy(tp->words[5], "1");
   strcpy(tp->words[6], "{");
   strcpy(tp->words[7], "}");

   turtle_do(tp);

   if(!strsame(tp->error, "Upper bound cannot be smaller than lower bound.\n")) {
      printf("I failed 1.\n");
      return false;
   }

   /* N.B. in interpreter, varnums must be integers
    * or evaluate to integers but this doesn't
    * matter in parser */

   reset_words(tp);
   reset_vartable(tp);
   strcpy(tp->words[1], "M");
   strcpy(tp->words[2], "FROM");
   strcpy(tp->words[3], "1.93");
   strcpy(tp->words[4], "TO");
   strcpy(tp->words[5], "-1.2");
   strcpy(tp->words[6], "{");
   strcpy(tp->words[7], "}");

   turtle_do(tp);

   if(!strsame(tp->error, "DO parameters have to be integers or variables set to integers.\n")) {
      printf("I failed 2.\n");
      return false;
   }

   reset_words(tp);
   reset_vartable(tp);
   strcpy(tp->words[1], "M");
   strcpy(tp->words[2], "from");
   strcpy(tp->words[3], "1.93");
   strcpy(tp->words[4], "TO");
   strcpy(tp->words[5], "-1.2");
   strcpy(tp->words[6], "{");
   strcpy(tp->words[7], "}");

   turtle_do(tp);

   if(!strsame(tp->error, "Expecting FROM in DO instruction.\n")) {
      printf("I failed 3.\n");
      return false;
   }

   reset_words(tp);
   reset_vartable(tp);
   strcpy(tp->words[1], "M");
   strcpy(tp->words[2], "FROM1");
   strcpy(tp->words[3], "1");
   strcpy(tp->words[4], "TO");
   strcpy(tp->words[5], "10");
   strcpy(tp->words[6], "{");
   strcpy(tp->words[7], "}");

   turtle_do(tp);

   if(!strsame(tp->error, "Expecting FROM in DO instruction.\n")) {
      printf("I failed 4.\n");
      return false;
   }

   reset_words(tp);
   reset_vartable(tp);
   strcpy(tp->words[1], "M");
   strcpy(tp->words[2], "FROM");
   strcpy(tp->words[3], "1");
   strcpy(tp->words[4], "To");
   strcpy(tp->words[5], "10");
   strcpy(tp->words[6], "{");
   strcpy(tp->words[7], "}");

   turtle_do(tp);

   if(!strsame(tp->error, "Expecting TO in DO instruction.\n")) {
      printf("I failed 5.\n");
      return false;
   }

   reset_words(tp);
   reset_vartable(tp);
   strcpy(tp->words[1], "M");
   strcpy(tp->words[2], "FROM");
   strcpy(tp->words[3], "1");
   strcpy(tp->words[4], "UNTIL");
   strcpy(tp->words[5], "10");
   strcpy(tp->words[6], "{");
   strcpy(tp->words[7], "}");

   turtle_do(tp);

   if(!strsame(tp->error, "Expecting TO in DO instruction.\n")) {
      printf("I failed 6.\n");
      return false;
   }

   reset_words(tp);
   reset_vartable(tp);
   strcpy(tp->words[1], "M");
   strcpy(tp->words[2], "FROM");
   strcpy(tp->words[3], "1");
   strcpy(tp->words[4], "TO");
   strcpy(tp->words[5], "10");
   strcpy(tp->words[6], "{{");
   strcpy(tp->words[7], "}");

   turtle_do(tp);

   if(!strsame(tp->error, "Expecting { in DO instruction.\n")) {
      printf("I failed 7.\n");
      return false;
   }

   reset_words(tp);
   reset_vartable(tp);
   strcpy(tp->words[1], "M");
   strcpy(tp->words[2], "FROM");
   strcpy(tp->words[3], "1");
   strcpy(tp->words[4], "TO");
   strcpy(tp->words[5], "10");
   strcpy(tp->words[6], "}");
   strcpy(tp->words[7], "}");

   turtle_do(tp);

   if(!strsame(tp->error, "Expecting { in DO instruction.\n")) {
      printf("I failed 8.\n");
      return false;
   }

   reset_words(tp);
   reset_vartable(tp);
   strcpy(tp->words[1], "K");
   strcpy(tp->words[2], "FROM");
   strcpy(tp->words[3], "1");
   strcpy(tp->words[4], "TO");
   strcpy(tp->words[5], "5");
   strcpy(tp->words[6], "{");
   strcpy(tp->words[7], "}");

   turtle_do(tp);
   var(tp, "read", "K");

   /* hack to get around error when using != with doubles */
   if(tp->vhelper.value > 5 || tp->vhelper.value < 5) {
      printf("I failed 9.\n");
      return false;
   }

   return true;

}

bool test_var(void) {

   Program test;
   Program* tp;
   tp = &test;
   tp->testing = 1;
   reset_words(tp);

   reset_vartable(tp);
   strcpy(tp->words[0], "A");

   assert(var(tp, "write", TESTWORD) == true);

   reset_vartable(tp);
   strcpy(tp->words[0], "Z");

   assert(var(tp, "write", TESTWORD) == true);

   reset_vartable(tp);
   strcpy(tp->words[0], "5");

   assert(var(tp, "read", TESTWORD) == false);

   assert(var(tp, "write", TESTWORD) == false);

   reset_vartable(tp);
   strcpy(tp->words[0], "3.78");

   assert(var(tp, "read", TESTWORD) == false);

   assert(var(tp, "write", TESTWORD) == false);

   reset_vartable(tp);
   strcpy(tp->words[0], "ABC");

   assert(var(tp, "read", TESTWORD) == false);

   assert(var(tp, "write", TESTWORD) == false);

   reset_vartable(tp);
   strcpy(tp->words[0], "q");

   assert(var(tp, "read", TESTWORD) == false);

   assert(var(tp, "write", TESTWORD) == false);

   reset_vartable(tp);
   strcpy(tp->words[0], "_K");

   assert(var(tp, "read", TESTWORD) == false);

   assert(var(tp, "write", TESTWORD) == false);

   reset_vartable(tp);
   strcpy(tp->words[0], "\0");

   assert(var(tp, "read", TESTWORD) == false);

   assert(var(tp, "write", TESTWORD) == false);

   reset_words(tp);
   reset_vartable(tp);

   strcpy(tp->words[1], "B");
   strcpy(tp->words[2], ":=");
   strcpy(tp->words[3], "2");
   strcpy(tp->words[4], "3");
   strcpy(tp->words[5], "+");
   strcpy(tp->words[6], ";");

   set(tp);

   var(tp, "read", "B");

   /* hack to get around error when using != with doubles */
   if(tp->vhelper.value > 5 || tp->vhelper.value < 5) {
      return false;
   }

   reset_words(tp);
   reset_vartable(tp);

   var(tp, "doodle", "D");

   if(!strsame(tp->error, "Read/write mode not recognised.\n")) {
      return false;
   }

   var(tp, "read", "Z");

   if(!strsame(tp->error, "The variable has not been declared.\n")) {
      return false;
   }

   return true;

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

   reset_words(tp);
   reset_vartable(tp);
   strcpy(tp->words[1], "Z");
   strcpy(tp->words[2], ":=");
   strcpy(tp->words[3], "3");
   strcpy(tp->words[4], "2");
   strcpy(tp->words[5], "-");
   strcpy(tp->words[6], ";");

   assert(set(tp) == 1);

   reset_words(tp);
   reset_vartable(tp);
   strcpy(tp->words[1], "C");
   strcpy(tp->words[2], "=:");

   set(tp);

   if(!strsame(tp->error, "The assignment operator is missing or incorrect.\n")) {
      return false;
   }

   reset_words(tp);
   reset_vartable(tp);
   strcpy(tp->words[1], "J");
   strcpy(tp->words[2], "36.2");

   set(tp);

   if(!strsame(tp->error, "The assignment operator is missing or incorrect.\n")) {
      return false;
   }

   reset_words(tp);
   reset_vartable(tp);
   strcpy(tp->words[1], "U");
   strcpy(tp->words[2], "\0");

   set(tp);

   if(!strsame(tp->error, "The assignment operator is missing or incorrect.\n")) {
      return false;
   }

   reset_words(tp);
   reset_vartable(tp);
   strcpy(tp->words[1], "P");
   strcpy(tp->words[2], "sheep");

   set(tp);

   if(!strsame(tp->error, "The assignment operator is missing or incorrect.\n")) {
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

void test_is_int(void) {

   assert(is_int(3) == true);
   assert(is_int(0) == true);
   assert(is_int(-80) == true);
   assert(is_int(3749) == true);
   assert(is_int(0.189) == false);
   assert(is_int(2.74) == false);
   assert(is_int(-13.32) == false);

   fprintf(stdout, "All is_int tests passed.\n");

}

void reset_words(Program *p) {

   int i;

   for(i = 0; i < MAXWORDS; i++) {
      strcpy(p->words[i], "\0");
   }

   p->cword = 0;
   strcpy(p->error, "\0");
}

void reset_vartable(Program *p) {

   int i;

   for(i = 0; i < MAXWORDS; i++) {
      p->vartbl[i].c = '\0';
      p->vartbl[i].d = 0.0;
   }

   p->vhelper.value = 0.0;
   p->vhelper.index = 0;
   p->vhelper.top = 0;
}

bool doubles_equal_buffer(double num1, double num2) {

   /* Note - this is used because one of the
    * compiler flags does not like comparing
    * doubles using == or != */

   if(num1 - num2 < BUFFER) {
      return true;
   }
   else if(num2 - num1 < BUFFER) {
      return true;
   }

   return false;

}
