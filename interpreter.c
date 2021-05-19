#include "interpreter.h"
#include "testinterpreter.h"

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

   prog.testing = TESTING;

   if(prog.testing) {
      test_all_functions();
      prog.testing = 0;

   }

    if(USINGSDL) {
        Neill_SDL_Init(&(prog.pos.win));
        do{

            SDL_Delay(MILLISECONDDELAY);

            Neill_SDL_SetDrawColour(&(prog.pos.win), 255, 255, 255);

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

void my_error(char err[ERRORSIZE], Program *p) {

   if(p->testing) {
      strcpy(p->error, err);
   }
   else {
      fprintf(stderr, "%s", err);
      exit(1);
   }
}

int inc_word(Program *p) {
    p->cword += 1;
    return 1;
}

int turtle_main(Program *p) {

    /* set values for position to default (middle of screen) */
    /* note angle of 0 means forwards goes right */

    int i;

    p->cword = 0;
    p->vhelper.index = 0;
    p->pos.angle = 0.0;
    p->pos.x = WWIDTH/2.0;
    p->pos.y = WHEIGHT/2.0;

    /* to avoid garbage matching variables
     * which would prevent us storing variables
     * in the variable table */

    for(i = 0; i < MAXWORDS; i++) {
        p->vartbl[i].c = '\0';
        p->vartbl[i].d = 0.0;
    }

    if(!strsame(CWORD, "{")) {
        my_error("A { is required to start the program.\n", p);
        return 1;
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
        if(p->testing == 0) {
            if(!forwards(p)) {
                fprintf(stderr, "Something went wrong with forwards function.\n");
                exit(1);
            }
        }
    }

    else if(strsame(CWORD, "LT") || strsame(CWORD, "RT")) {
        if(p->testing == 0) {
            if(!turn(p)) {
                fprintf(stderr, "Something went wrong with turn function.\n");
                exit(1);
            }
        }
    }

    else if(strsame(CWORD, "DO")) {
        if(p->testing == 0) {
            if(!turtle_do(p)) {
                fprintf(stderr, "Something went wrong with turtle_do function.\n");
                exit(1);
            }
        }
    }

    else if(strsame(CWORD, "SET")) {
        if(p->testing == 0) {
            if(!set(p)) {
                fprintf(stderr, "Something went wrong with set function.\n");
                exit(1);
            }
        }
    }

    else {
        my_error("The instruction has not been recognised.\n", p);
        return 0;
    }

    return 1;

}

int forwards(Program *p) {

    /* already checked that FD is the current word */

    double amount, x_before, y_before, x_after, y_after;

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
    }
    else {
        amount = atof(CWORD);
    }

    if(DEBUG){
        fprintf(stdout, "Before x coord = %.2f\n", p->pos.x);
        fprintf(stdout, "Before y coord = %.2f\n", p->pos.y);
    }

    x_before = p->pos.x;
    y_before = p->pos.y;

    p->pos.x += SWITCH * cos(p->pos.angle) * amount;
    p->pos.y += SWITCH * sin(p->pos.angle) * amount;

    x_after = p->pos.x;
    y_after = p->pos.y;

    if(USINGSDL) {
        SDL_RenderDrawLine(p->pos.win.renderer, (int) x_before, (int) y_before, (int) x_after, (int) y_after);
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
        my_error("Expecting FROM in DO instruction.\n", p);
        return 0;
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
            my_error("DO parameters have to be integers or variables set to integers.\n", p);
            return 0;
        }
    }
    else {
        my_error("DO parameters have to be integers or variables set to integers.\n", p);
        return 0;
    }

    if(!inc_word(p)) {
        fprintf(stderr, "Something went wrong with inc_word function.\n");
        exit(1);
    }

    if(!strsame(CWORD, "TO")) {
        my_error("Expecting TO in DO instruction.\n", p);
        return 0;
    }

    if(!inc_word(p)) {
        fprintf(stderr, "Something went wrong with inc_word function.\n");
        exit(1);
    }

    if(varnum(p)) {

        if(var(p, "read", p->words[p->cword])) {
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
                fprintf(stdout, "Upper bound is %d.\n", (int) upperbound);
            }
        }
        else {
            my_error("DO parameters have to be integers or variables set to integers.\n", p);
            return 0;
        }

        if(upperbound < lowerbound) {
            my_error("Upper bound cannot be smaller than lower bound.\n", p);
            return 0;
        }
    }
    else {
        my_error("DO parameters have to be integers or variables set to integers.\n", p);
        return 0;
    }

    if(!inc_word(p)) {
        fprintf(stderr, "Something went wrong with inc_word function.\n");
        exit(1);
    }

    if(!strsame(CWORD, "{")) {
        my_error("Expecting { in DO instruction.\n", p);
        return 0;
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
            my_error("The variable has not been declared.\n", p);
            return false;
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
        my_error("Read/write mode not recognised.\n", p);
        return false;
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
                    my_error("This is not a valid variable or number.\n", p);
                    return false;
                }
            }
            else {
                my_error("This is not a valid variable or number.\n", p);
                return false;
            }
        }
        else {
            my_error("This is not a valid variable or number.\n", p);
            return false;
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
                my_error("This is not a valid variable or number.\n", p);
                return false;
            }
        }
        else {
            my_error("This is not a valid variable or number.\n", p);
            return false;
        }
    }
    /* if first character is not a digit or a
    * negative sign, this is not a number */
    else {
        my_error("This is not a valid variable or number.\n", p);
        return false;
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
        my_error("The assignment operator is missing or incorrect.\n", p);
        return 0;
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
            if (operator == '/') {
                if (!(operand2 > 0.0 || operand2 < 0.0)) {
                    my_error("Cannot divide by zero.\n", p);
                    return 0;
                }
                else {
                    result = operand1 / operand2;
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
            my_error("Polish notation passed parser check but cannot be interpreted.\n", p);
            return 0;
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
        my_error("Not a valid operation, variable or number or no terminating semicolon.\n", p);
        return 0;
    }

    return 1;
}

bool op(Program *p) {

    int n = strlen(CWORD);

    if(n > 1) {
        return false;
    }

    if(CWORD[0] == '+' || CWORD[0] == '-' || CWORD[0] == '*' || CWORD[0] == '/') {
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

    if(num > 0) {
        if (check >= num) {
            return true;
        } else {
            return false;
        }
    }
    else {
        if (check <= num) {
            return true;
        } else {
            return false;
        }
    }
}
