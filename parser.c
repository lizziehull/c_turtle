#include "parser.h"
#include "testparser.h"

int main(int argc, char** argv) {

    int i;
    FILE *fp;
    Program prog;

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

    if(turtle_main(&prog)) {
        if(DEBUG) {
        fprintf(stdout, "Parsed successfully.\n");
        }
    }
    else {
        fprintf(stderr, "Error with the turtle_main function.\n");
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

    p->cword = 0;

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

    if(!inc_word(p)) {
        fprintf(stderr, "Something went wrong with inc_word function.\n");
        exit(1);
    }

    if(!varnum(p)) {
        fprintf(stderr, "Please enter a valid variable or number.\n");
        exit(1);
    }

    return 1;
}

int turn(Program *p) {

    /* already checked that LT or RT is the current word */

    if(!inc_word(p)) {
        fprintf(stderr, "Something went wrong with inc_word function.\n");
        exit(1);
    }

    if(!varnum(p)) {
        fprintf(stderr, "Please enter a valid variable or number.\n");
        exit(1);
    }

    return 1;

}

int turtle_do(Program *p) {

    /* already checked that DO is the current word */

    if(!inc_word(p)) {
        fprintf(stderr, "Something went wrong with inc_word function.\n");
        exit(1);
    }

    if (!var(p)) {
        fprintf(stderr, "Please enter a valid variable.\n");
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

    if(!varnum(p)) {
        fprintf(stderr, "Please enter a valid variable or number.\n");
        exit(1);
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

    if(!varnum(p)) {
        fprintf(stderr, "Please enter a valid variable or number.\n");
        exit(1);
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

    if(!instruction_list(p)) {
        fprintf(stderr, "Something went wrong with instruction_list function.\n");
        exit(1);
    }

    return 1;

}

bool var(Program *p) {

    /* variable has to be single digit and also
     * uppercase because the formal grammar specified
     * [A-Z] - I assume this was regex notation */

    int n = strlen(CWORD);

    if(n > 1) {
        return false;
    }

    if(!isupper(CWORD[0])) {
        return false;
    }

    return true;
}

bool varnum(Program *p) {

    int i;
    int pointcnt = 0, digitcnt = 0;
    int n = strlen(CWORD);

    /* check if it is a variable */
    if(var(p)) {
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

    if(!inc_word(p)) {
        fprintf(stderr, "Something went wrong with inc_word function.\n");
        exit(1);
    }

    if(!var(p)) {
        fprintf(stderr, "Please enter a valid variable.");
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
        fprintf(stderr, "Something has gone wrong with polish function.\n");
        exit(1);
    }

    return 1;
}

int polish(Program *p) {

    if(strsame(CWORD, ";")) {
        return 1;
    }

    /* okay to combine for parser but doesn't
     * work for interpreter - see interpreter.c */
    if(op(p) || varnum(p)) {

        if(!inc_word(p)) {
            fprintf(stderr, "Something has gone wrong with inc_word function.\n");
            exit(1);
        }

        if(!polish(p)) {
            fprintf(stderr, "Something has gone wrong with polish function.\n");
            exit(1);
        }
    }
    else {
        my_error("Not a valid operation, variable or number or no terminating semicolon.\n", p);
        return 1;
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
