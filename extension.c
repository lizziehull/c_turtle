#include "extension.h"

int main(int argc, char** argv) {

   int i, done = 0;
   FILE *fp;
   Program prog;

   if(!initialise_stack(&(prog.stk))) {
      fprintf(stderr, "Initialising the stack has failed.\n");
      exit(1);
   }

   for(i = 0; i < MAXWORDS; i++) {
      prog.words[i][0] = '\0';
   }
   if(argc == 2) {
      fp = fopen(argv[1], "r");
   }
   else {
      fprintf(stderr, "Please include exactly one text file to be parsed.\n");
      exit(1);
   }
   if(fp == NULL) {
      fprintf(stderr, "An error has occurred in reading the file.\n");
      exit(1);
   }

   i = 0;

   while(fscanf(fp, "%s", prog.words[i++]) == 1 && i < MAXWORDS);
   fclose(fp);

   assert(i < MAXWORDS);

   if(USINGSDL) {
      Neill_SDL_Init(&(prog.pos.win));
      do{

         SDL_Delay(MILLISECONDDELAY);

         Neill_SDL_UpdateScreen(&(prog.pos.win));

         /* Neill's comment from blocks.c:
          * Has anyone pressed ESC or killed the SDL window ?
          *  Must be called frequently - it's the only way of escaping  */
         Neill_SDL_Events(&(prog.pos.win));

         if(!done) {
            if(turtle_main(&prog)) {
               if(DEBUG) {
                  fprintf(stdout, "Parsed and interpreted successfully.\n");
               }
               done++;
            }
            else {
               fprintf(stderr, "Error with the turtle_main function.\n");
               exit(1);
            }
         }

      }while(!(prog.pos.win).finished);

      /* Neill's comment from blocks.c:
       * Clear up graphics subsystems */
      SDL_Quit();
      atexit(SDL_Quit);
   }
   else {
      if(turtle_main(&prog)) {
         if(DEBUG){
            fprintf(stdout, "Parsed and interpreted successfully.\n");
         }
      }
      else {
         fprintf(stderr, "Error with the turtle_main function.\n");
         exit(1);
      }
   }

   return 0;
}

int inc_word(Program *p) {
   p->cword += 1;
   return 1;
}

int turtle_main(Program *p) {

   /* set values for position to default (middle of screen) */
   /* note angle of 0 means forwards goes right */

   int i;

   time_t now;
   now = time(0);
   srand(now);

   p->cword = 0;
   p->vhelper.index = 0;
   p->pos.angle = 0.0;
   p->pos.x = WWIDTH/2.0;
   p->pos.y = WHEIGHT/2.0;
   p->pos.colour_set = 0;
   p->pos.pen = true;
   p->pos.dash = 0;

   /* to avoid garbage matching variables
    * which would prevent us storing variables
    * in the variable table */

   for(i = 0; i < MAXWORDS; i++) {
      p->vartbl[i].c = '\0';
      p->vartbl[i].d = 0.0;
   }

   if(!strsame(CWORD, "{")) {
      fprintf(stderr, "A { is required to start the program.\n");
      exit(1);
   }

   if(!inc_word(p)) {
      fprintf(stderr, "Something went wrong with inc_word function.\n");
      exit(1);
   }

   if(!instruction_list(p)) {
      fprintf(stderr, "Something went wrong with instruction_list function.\n");
      exit(1);
   }

   return 1;
}

int instruction_list(Program *p) {

   if(strsame(CWORD, "}")) {
      return 1;
      /* program only terminates with an "}" */
   }

   if(!instruction(p)) {
      fprintf(stderr, "Something went wrong with instruction function.\n");
      exit(1);
   }

   if(!inc_word(p)) {
      fprintf(stderr, "Something went wrong with inc_word function.\n");
      exit(1);
   }

   if(!instruction_list(p)) {
      fprintf(stderr, "Something went wrong with instruction_list function.\n");
      exit(1);
   }

   return 1;
}

int instruction(Program *p) {

   if(strsame(CWORD, "FD")) {
      if(!forwards(p)) {
         fprintf(stderr, "Something went wrong with forwards function.\n");
         exit(1);
      }
   }

   else if(strsame(CWORD, "LT") || strsame(CWORD, "RT")) {
      if(!turn(p)) {
         fprintf(stderr, "Something went wrong with turn function.\n");
         exit(1);
      }
   }

   else if(strsame(CWORD, "DO")) {
      if(!turtle_do(p)) {
         fprintf(stderr, "Something went wrong with turtle_do function.\n");
         exit(1);
      }
   }

   else if(strsame(CWORD, "SET")) {
      if(!set(p)) {
         fprintf(stderr, "Something went wrong with set function.\n");
         exit(1);
      }
   }

   else if(strsame(CWORD, "COLOUR")) {
      if(!select_colour(p)) {
         fprintf(stderr, "Something went wrong with colour function.\n");
         exit(1);
      }
   }

   else if(strsame(CWORD, "PENUP")) {
      if(!penup(p)) {
         fprintf(stderr, "Something went wrong with penup function.\n");
         exit(1);
      }
   }

   else if(strsame(CWORD, "PENDOWN")) {
      if(!pendown(p)) {
         fprintf(stderr, "Something went wrong with pendown function.\n");
         exit(1);
      }
   }

   else if(strsame(CWORD, "DASH")) {
      if(!dashline(p)) {
         fprintf(stderr, "Something went wrong with dashline function.\n");
         exit(1);
      }
   }

   else if(strsame(CWORD, "IF")) {
      if(!conditional(p)) {
         fprintf(stderr, "Something went wrong with conditional function.\n");
         exit(1);
      }
   }

   else {
      fprintf(stderr, "The instruction has not been recognised.\n");
      exit(1);
   }

   return 1;

}

int forwards(Program *p) {

   /* already checked that FD is the current word */

   double amount, x_before, y_before, x_after, y_after,
   x_dash1, y_dash1, x_dash2, y_dash2, leftover;
   int dash, cnt = 0, amount_int;

   if(!inc_word(p)) {
      fprintf(stderr, "Something went wrong with inc_word function.\n");
      exit(1);
   }

   if(!varnum(p)) {
      fprintf(stderr, "Please enter a valid variable or number.\n");
      exit(1);
   }

   if(var(p, "read", CWORD)) {
      amount = p->vhelper.value;
      amount_int = (int) amount;
   }
   else {
      amount = atof(CWORD);
      amount_int = (int) amount;
   }

   if(DEBUG){
      fprintf(stdout, "Before x coord = %.2f\n", p->pos.x);
      fprintf(stdout, "Before y coord = %.2f\n", p->pos.y);
   }

   x_before = p->pos.x;
   p->pos.dash_x = p->pos.x;
   x_dash1 = p->pos.dash_x;

   y_before = p->pos.y;
   p->pos.dash_y = p->pos.y;
   y_dash1 = p->pos.dash_y;

   p->pos.x += SWITCH * SCALE * cos(p->pos.angle) * amount;
   p->pos.y += SWITCH * SCALE * sin(p->pos.angle) * amount;

   x_after = p->pos.x;
   y_after = p->pos.y;

   if(USINGSDL) {
      if(!(p->pos.colour_set)) {
         Neill_SDL_SetDrawColour(&(p->pos.win), 255, 255, 255);
         /* default colour is white */
      }
      if((!p->pos.dash && p->pos.pen) || (p->pos.pen && p->pos.dash >= amount_int)) {
         SDL_RenderDrawLine(p->pos.win.renderer, (int) x_before, (int) y_before, (int) x_after, (int) y_after);
      }
      else if(p->pos.pen && p->pos.dash > 0 && p->pos.dash < amount_int) {

         dash = p->pos.dash;

         x_dash1 = x_before;
         y_dash1 = y_before;

         while (amount_int >= dash) {

            p->pos.dash_x += SWITCH * SCALE * cos(p->pos.angle) * dash;
            p->pos.dash_y += SWITCH * SCALE * sin(p->pos.angle) * dash;

            x_dash2 = p->pos.dash_x;
            y_dash2 = p->pos.dash_y;

            if(DEBUG) {
               fprintf(stdout,
                       "Before is x = %f, y = %f, after is x = %f, y = %f, dash is %d, count is %d.\n",
                       x_dash1, y_dash1, x_dash2, y_dash2, dash, cnt);
            }

            if(cnt % DASHGAP == 0) {
               SDL_RenderDrawLine(p->pos.win.renderer, (int) x_dash1, (int) y_dash1, (int) x_dash2, (int) y_dash2);
            }

            cnt++;

            x_dash1 = x_dash2;
            y_dash1 = y_dash2;

            amount_int -= dash;

         }

         leftover = dash - amount_int;
         if(leftover) {

            x_dash2 = x_after;
            y_dash2 = y_after;

            if(cnt % 2 == 0) {
               SDL_RenderDrawLine(p->pos.win.renderer, (int) x_dash1, (int) y_dash1, (int) x_dash2, (int) y_dash2);
            }
         }

      }
      else if(p->pos.dash < 0) {
         fprintf(stderr, "Size of dashline cannot be a negative integer.\n");
      }

   }

   if(DEBUG) {
      /* N.B Angle is stored as radians but printed as degrees */
      fprintf(stdout, "Angle = %.2f\n", (p->pos.angle) * (180 / PI));
      fprintf(stdout, "After x coord = %.2f\n", p->pos.x);
      fprintf(stdout, "After y coord = %.2f\n\n", p->pos.y);
   }

   return 1;
}

int turn(Program *p) {

   /* already checked that LT or RT is the current word */

   int dir = 1;
   double theta;

   /* reverses direction if a right turn */
   if(strsame(p->words[p->cword], "RT")) {
      dir = -1;
   }

   if(!inc_word(p)) {
      fprintf(stderr, "Something went wrong with inc_word function.\n");
      exit(1);
   }

   if(!varnum(p)) {
      fprintf(stderr, "Please enter a valid variable or number.\n");
      exit(1);
   }

   if(var(p, "read", CWORD)) {
      theta = p->vhelper.value;
   }
   else {
      theta = atof(CWORD);
   }

   /* assume input is degrees
    * convert from degrees to radians */
   theta *= (PI / 180);

   if(DEBUG) {
      fprintf(stdout, "Angle before is %.2f\n", (p->pos.angle) * (180 / PI));
   }

   p->pos.angle += dir * theta;

   if(DEBUG) {
      fprintf(stdout, "Angle after is %.2f\n\n", (p->pos.angle) * (180 / PI));
   }

   return 1;

}

int turtle_do(Program *p) {

   /* already checked that DO is the current word */

   char tmp[MAXCMDSIZE];
   int i, idx, save_word;
   double lowerbound = SETLB, upperbound;


   if(!inc_word(p)) {
      fprintf(stderr, "Something went wrong with inc_word function.\n");
      exit(1);
   }

   if (var(p, "write", CWORD)) {
      idx = p->vhelper.index;
   }
   else {
      fprintf(stderr, "DO requires a variable at the start.\n");
      exit(1);
   }

   if(!inc_word(p)) {
      fprintf(stderr, "Something went wrong with inc_word function.\n");
      exit(1);
   }

   if(!strsame(CWORD, "FROM")) {
      fprintf(stderr, "Expecting FROM in DO instruction.\n");
      exit(1);
   }

   if(!inc_word(p)) {
      fprintf(stderr, "Something went wrong with inc_word function.\n");
      exit(1);
   }

   if(varnum(p)) {

      if(var(p, "read", CWORD)) {
         if(sprintf(tmp, "%f", p->vhelper.value) < 0) {
            fprintf(stderr, "Something went wrong with sprintf.\n");
            exit(1);
         }
      }
      else {
         strcpy(tmp, CWORD);
      }

      lowerbound = atof(tmp);

      if(is_int(lowerbound)) {
         if(DEBUG) {
            fprintf(stdout, "Lower bound is %d.\n", (int) lowerbound);
         }
      }
      else {
         fprintf(stderr, "DO parameters have to be integers or variables set to integers.\n");
         exit(1);
      }
   }
   else {
      fprintf(stderr, "DO parameters have to be integers or variables set to integers.\n");
   }

   if(!inc_word(p)) {
      fprintf(stderr, "Something went wrong with inc_word function.\n");
      exit(1);
   }

   if(!strsame(CWORD, "TO")) {
      fprintf(stderr, "Expecting TO in DO instruction.\n");
      exit(1);
   }

   if(!inc_word(p)) {
      fprintf(stderr, "Something went wrong with inc_word function.\n");
      exit(1);
   }

   if(varnum(p)) {

      if(var(p, "read", p->words[p->cword])) {
         /* printf("%f\n", p->value); */
         if(sprintf(tmp, "%f", p->vhelper.value) < 0) {
            fprintf(stderr, "Something went wrong with sprintf function.\n");
            exit(1);
         }
      }
      else {
         strcpy(tmp, p->words[p->cword]);
      }

      upperbound = atof(tmp);

      if(is_int(upperbound)) {
         if(DEBUG) {
            fprintf(stdout, "Lower bound is %d.\n", (int) upperbound);
         }
      }
      else {
         fprintf(stderr, "DO parameters have to be integers or variables set to integers.\n");
         exit(1);
      }

      if(upperbound - lowerbound < 0) {
         fprintf(stderr, "Upper bound cannot be smaller than lower bound.\n");
         exit(1);
      }
   }
   else {
      fprintf(stderr, "DO parameters have to be integers or variables set to integers.\n");
      exit(1);
   }

   if(!inc_word(p)) {
      fprintf(stderr, "Something went wrong with inc_word function.\n");
      exit(1);
   }

   if(!strsame(CWORD, "{")) {
      fprintf(stderr, "Expecting { in DO instruction.\n");
      exit(1);
   }

   if(!inc_word(p)) {
      fprintf(stderr, "Something went wrong with inc_word function.\n");
      exit(1);
   }

   for(i = (int) lowerbound; i <= (int) upperbound; i++) {

      save_word = p->cword;

      if(DEBUG) {
         printf("Doing loop with var set to %d.\n", i);
      }

      p->vartbl[idx].d = i;

      if(!instruction_list(p)) {
         fprintf(stderr, "Something went wrong with instruction_list function.\n");
         exit(1);
      }

      if(i < upperbound) {
         /* resets to start of loop
          * not wanted when loop has finished */
         p->cword = save_word;
      }
   }

   return 1;

}

bool var(Program *p, char* mode, char* word) {

   /* variable has to be single digit and also
    * uppercase because the formal grammar specified
    * [A-Z] - I assume this was regex notation */

   int i, tmp, found = 0, n = strlen(word);
   char c = word[0];

   if(n > 1) {
      return false;
   }

   if(!isupper(c)) {
      return false;
   }

   tmp = p->vhelper.index;

   for(i = 0; i <= p->vhelper.top; i++) {
      if(!found) {
         if(p->vartbl[i].c == c) {
            found++;
            p->vhelper.index = i;
         }
      }
   }

   if(strsame(mode, "read")) {
      if(found) {
         p->vhelper.value = p->vartbl[p->vhelper.index].d;
         p->vhelper.index = tmp;
      }
      else {
         fprintf(stderr, "The variable has not been declared.");
         exit(1);
      }
   }
   else if(strsame(mode, "write")) {
      if(!found) {
         p->vartbl[p->vhelper.top].c = c;
         p->vhelper.index = p->vhelper.top;
         p->vhelper.top += 1;
      }
   }
      /* if found we don't want it written
       * a second time, so we do nothing */
   else {
      fprintf(stderr, "Read/write mode not recognised.\n");
      exit(1);
   }

   return true;

}

bool varnum(Program *p) {

   int i;
   int pointcnt = 0, digitcnt = 0;
   int n = strlen(CWORD);

   /* check if it is a variable */
   if(var(p, "read", CWORD)) {
      return true;
   }

   /* check if it is a negative int or float */
   if(CWORD[0] == '-') {
      /* check there is at least one digit between
       * the '-' and the decimal point, if there is one */
      if(isdigit(CWORD[1])) {
         for(i = 2; i < n; i++) {
            if(CWORD[i] == '.') {
               pointcnt++;
            }
            else if(isdigit(CWORD[i])) {
               digitcnt++;
            }
         }
         /* n - 2 because of '-', first digit and no point */
         if(pointcnt == 0 && digitcnt == (n - 2)) {
            return true;
         }
            /* n - 3 because of '-', first digit and point
             * check at least one digit after point */
         else if(pointcnt == 1 && digitcnt == (n - 3)) {
            if(CWORD[n - 1] != '.') {
               return true;
            }
            else {
               fprintf(stderr, "This is not a valid variable or number.\n");
               exit(1);
            }
         }
         else {
            fprintf(stderr, "This is not a valid variable or number.\n");
            exit(1);
         }
      }
      else {
         fprintf(stderr, "This is not a valid variable or number.\n");
         exit(1);
      }
   }
      /* check if it is a positive int or float */
      /* check there is at least one digit before
       * the decimal point, if there is one */
   else if(isdigit(CWORD[0])) {
      for(i = 1; i < n; i++) {
         if(CWORD[i] == '.') {
            pointcnt++;
         }
         else if(isdigit(CWORD[i])) {
            digitcnt++;
         }
      }
      /* n - 1 because of first digit and no point */
      if(pointcnt == 0 && digitcnt == (n - 1)) {
         return true;
      }
         /* n - 2 because of first digit and point
          * check at least one digit after point */
      else if(pointcnt == 1 && digitcnt == (n - 2)) {
         if(CWORD[n - 1] != '.') {
            return true;
         }
         else {
            fprintf(stderr, "This is not a valid variable or number.\n");
            exit(1);
         }
      }
      else {
         fprintf(stderr, "This is not a valid variable or number.\n");
         exit(1);
      }
   }
      /* if first character is not a digit or a
      * negative sign, this is not a number */
   else {
      fprintf(stderr, "This is not a valid variable or number.\n");
      exit(1);
   }

}

int set(Program *p) {

   /* already checked current word is SET */

   char tmp[MAXCMDSIZE];

   if(!inc_word(p)) {
      fprintf(stderr, "Something went wrong with inc_word function.\n");
      exit(1);
   }

   if(!var(p, "write", CWORD)) {
      fprintf(stderr, "Please enter a valid variable.\n");
      exit(1);
   }

   if(!inc_word(p)) {
      fprintf(stderr, "Something went wrong with inc_word function.\n");
      exit(1);
   }

   if(!strsame(CWORD, ":=")) {
      fprintf(stderr, "The assignment operator is missing or incorrect.\n");
      exit(1);
   }

   if(!inc_word(p)) {
      fprintf(stderr, "Something went gone wrong with inc_word function.\n");
      exit(1);
   }

   if(!polish(p)) {
      fprintf(stderr, "Something went wrong with polish function.\n");
      exit(1);
   }

   if(!pop(&(p->stk), tmp)) {
      fprintf(stderr, "Something went wrong with pop function.\n");
      exit(1);
   }

   p->vartbl[p->vhelper.index].d = atof(tmp);

   return 1;
}

int polish(Program *p) {

   double operand1, operand2, result;
   char operator;
   char tmp[MAXCMDSIZE], tmp2[MAXCMDSIZE], buffer[MAXCMDSIZE];

   if(strsame(CWORD, ";")) {
      return 1;
   }

   if(op(p)) {

      operator = CWORD[0];

      if(!pop(&(p->stk), tmp2)) {
         fprintf(stderr, "Something went wrong with pop function.\n");
         exit(1);
      }

      if(!pop(&(p->stk), tmp)) {
         fprintf(stderr, "Something went wrong with pop function.\n");
         exit(1);
      }

      if (!strsame(tmp, "ERROR") && !strsame(tmp2, "ERROR")) {

         if(var(p, "read", tmp)) {
            if(sprintf(tmp, "%f", p->vhelper.value) < 0) {
               fprintf(stderr, "Something went wrong with sprintf function.\n");
               exit(1);
            }
         }
         if(var(p,"read", tmp2)) {
            if(sprintf(tmp, "%f", p->vhelper.value) < 0) {
               fprintf(stderr, "Something went wrong with sprintf function.\n");
               exit(1);
            }
         }

         operand1 = atof(tmp);
         operand2 = atof(tmp2);

         if(operator == '+') {
            result = operand1 + operand2;
         }
         if(operator == '-') {
            result = operand1 - operand2;
         }
         if(operator == '*') {
            result = operand1 * operand2;
         }
         if(operator == '/') {
            if (!(operand2 > 0.0 || operand2 < 0.0)) {
               fprintf(stderr, "Cannot divide by zero.\n");
            }
            else {
               result = operand1 / operand2;
            }
         }
         if(operator == '%') {
            if(is_int(operand1) && is_int(operand2)) {
               result = (int) operand1 % (int) operand2;
            }
            else {
               fprintf(stderr, "The modulus operator only works with integers.\n");
               exit(1);
            }
         }

         if(sprintf(buffer, "%f", result) < 0) {
            fprintf(stderr, "Something went wrong with sprintf function.\n");
            exit(1);
         }

         if(!push(&(p->stk), buffer)) {
            fprintf(stderr, "Something went wrong with push function.\n");
            exit(1);
         }

         if(!inc_word(p)) {
            fprintf(stderr, "Something has gone wrong with inc_word function.\n");
            exit(1);
         }

         if(!polish(p)) {
            fprintf(stderr, "Something has gone wrong with polish function.\n");
            exit(1);
         }

      } else {
         fprintf(stderr, "Polish notation passed parser check but cannot be interpreted.\n");
         exit(1);
      }
   }
   else if(varnum(p)) {

      if(!push(&(p->stk), CWORD)) {
         fprintf(stderr, "Something went wrong with push function.\n");
         exit(1);
      }

      if(!inc_word(p)) {
         fprintf(stderr, "Something has gone wrong with inc_word function.\n");
      }

      if(!polish(p)) {
         fprintf(stderr, "Something has gone wrong with polish function.\n");
      }
   }
   else {
      fprintf(stderr, "Please enter a valid operation, variable or number.\n");
      exit(1);
   }

   return 1;
}

bool op(Program *p) {

   int n = strlen(CWORD);

   if(n > 1) {
      return false;
   }

   if(CWORD[0] == '+' || CWORD[0] == '-' || CWORD[0] == '*' || CWORD[0] == '/' || CWORD[0] == '%') {
      return true;
   }

   return false;
}

int initialise_stack(Stack *s) {
   s->tp = NULL;
   return 1;
}

int push(Stack *s, char n[MAXCMDSIZE]) {
   Elem *e;
   e = (Elem *) calloc(1, sizeof(Elem));
   strcpy(e->c, n);
   e->prev = s->tp;
   s->tp = e;

   return 1;
}

int pop(Stack *s, char rd[MAXCMDSIZE]) {

   Elem *e;

   if(s->tp != NULL) {
      strcpy(rd, s->tp->c);
      e = s->tp;
      s->tp = s->tp->prev;
      free(e);
   }
   else {
      strcpy(rd, "ERROR");
   }

   return 1;
}

bool is_int(double num) {

   int check;
   check = (int) num;

   if(num <= (double) check) {
      return true;
   }

   return false;
}

int select_colour(Program *p) {

   /* already checked that COLOUR is the current word */

   if(!inc_word(p)) {
      fprintf(stderr, "Something has gone wrong with inc_word function.\n");
      exit(1);
   }

   if(USINGSDL) {

      if (strsame(CWORD, "white")) {
         Neill_SDL_SetDrawColour(&(p->pos.win), 255, 255, 255);
         p->pos.colour_set = 1;
      }
      else if (strsame(CWORD, "red")) {
         Neill_SDL_SetDrawColour(&(p->pos.win), 255, 0, 0);
         p->pos.colour_set = 1;
      }
      else if (strsame(CWORD, "green")) {
         Neill_SDL_SetDrawColour(&(p->pos.win), 0, 255, 0);
         p->pos.colour_set = 1;
      }
      else if (strsame(CWORD, "blue")) {
         Neill_SDL_SetDrawColour(&(p->pos.win), 0, 0, 255);
         p->pos.colour_set = 1;
      }
      else if(strsame(CWORD, "random")) {
         Neill_SDL_SetDrawColour(&(p->pos.win), rand() % 256, rand() % 256, rand() % 256);
         p->pos.colour_set = 1;
      }
      else {
         fprintf(stderr, "Colour is not recognized.\n");
      }
   }

   else {
      if(DEBUG) {
         fprintf(stdout, "Colour chosen is %s.\n", CWORD);
      }
   }

   return 1;

}

int penup(Program *p) {

   p->pos.pen = false;

   return 1;
}

int pendown(Program *p) {

   p->pos.pen = true;

   return 1;

}

int dashline(Program *p) {

   /* already checked current word is DASHLINE */

   if(!inc_word(p)) {
      fprintf(stderr, "Something has gone wrong with inc_word function.\n");
      exit(1);
   }

   if(var(p, "read", CWORD)) {
       if(is_int(p->vhelper.value)) {
          p->pos.dash = (int) p->vhelper.value;
       }
       else {
          fprintf(stderr, "Dashline variables need to be set to integers.\n");
          exit(1);
       }
   }
   else {
      if(is_int(atof(CWORD))) {
         p->pos.dash = (int) atof(CWORD);
      }
      else {
         fprintf(stderr, "Dashline numbers must be integers.\n");
      }
   }

   return 1;

}

int conditional(Program *p) {

   /* already checked current word is IF */

   int condtrue = 0;
   double comp1, comp2;
   char condop[MAXCMDSIZE];

   if(!inc_word(p)) {
      fprintf(stderr, "Something went wrong with inc_word function.\n");
      exit(1);
   }

   if(varnum(p)) {

      if (var(p, "read", CWORD)) {
         comp1 = p->vhelper.value;
      }
      else {
         comp1 = atof(CWORD);
      }
   }
   else {
      fprintf(stderr, "LHS of conditional not a valid variable or number.\n");
      exit(1);
   }


   if(!inc_word(p)) {
      fprintf(stderr, "Something went wrong with inc_word function.\n");
      exit(1);
   }

   if(!conditional_op(p)) {
      fprintf(stderr, "Please choose a valid comparison operator.\n");
      exit(1);
   }

   strcpy(condop, CWORD);

   if(!inc_word(p)) {
      fprintf(stderr, "Something went wrong with inc_word function.\n");
      exit(1);
   }

   if(varnum(p)) {

      if (var(p, "read", CWORD)) {
         comp2 = p->vhelper.value;
      }
      else {
         comp2 = atof(CWORD);
      }
   }
   else {
      fprintf(stderr, "RHS of conditional not a valid variable or number.\n");
      exit(1);
   }

   if(strsame(condop, ">")) {
      if(comp1 > comp2) {
         condtrue++;
      }
   }
   else if(strsame(condop, ">=")) {
      if(comp1 >= comp2) {
         condtrue++;
      }
   }
   else if(strsame(condop, "<")) {
      if(comp1 < comp2) {
         condtrue++;
      }
   }
   else if(strsame(condop, "<=")) {
      if(comp1 <= comp2) {
         condtrue++;
      }
   }
   else if(strsame(condop, "==")) {
      if(!(comp1 - comp2 < 0.0 || comp1 - comp2 > 0.0)) {
         condtrue++;
      }
   }
   else if(strsame(condop, "!=")) {
      if(comp1 - comp2 < 0.0 || comp1 - comp2 > 0.0) {
         condtrue++;
      }
   }

   if(!inc_word(p)) {
      fprintf(stderr, "Something went wrong with inc_word function.\n");
      exit(1);
   }

   if(!strsame(CWORD, "{")) {
      fprintf(stderr, "Expecting { in IF instruction.\n");
      exit(1);
   }

   if(!inc_word(p)) {
      fprintf(stderr, "Something went wrong with inc_word function.\n");
      exit(1);
   }

   if(condtrue) {
      if(!instruction_list(p)) {
         fprintf(stderr, "Something went wrong with instruction_list function.\n");
         exit(1);
      }
   }
   else {
      while(!strsame(CWORD, "}")) {
         if(!inc_word(p)) {
            fprintf(stderr, "Something went wrong with inc_word function.\n");
            exit(1);
         }
      }
   }

   return 1;

}

bool conditional_op(Program *p) {

   if(strsame(CWORD, ">") || strsame(CWORD, ">=")
   || strsame(CWORD, "<")  || strsame(CWORD, "<=")
   || strsame(CWORD, "==") || strsame(CWORD, "!=")) {
      return true;
    }

   return false;

}


