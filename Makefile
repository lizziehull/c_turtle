CC = gcc
LIBS = -lm
CFLAGS = -Wall -Wextra -Werror -Wfloat-equal -pedantic -ansi $(LIBS)
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs)
DEBUG = -g3
OPTIM = -O2
VALGRIND = --error-exitcode=1 --quiet --leak-check=full
GRAMMAR = test.txt

parse: parser.c parser.h testparser.c testparser.h
	$(CC) parser.c testparser.c -o parse $(CFLAGS) $(OPTIM)

interp: interpreter.c interpreter.h testinterpreter.c testinterpreter.h neillsdl2.c neillsdl2.h
	$(CC) interpreter.c testinterpreter.c neillsdl2.c -o interp $(SDL_CFLAGS) $(SDL_LDFLAGS) $(CFLAGS) $(OPTIM)

extension: extension.c extension.h neillsdl2.c neillsdl2.h
	$(CC) extension.c neillsdl2.c -o extension $(SDL_CFLAGS) $(SDL_LDFLAGS) $(CFLAGS) $(OPTIM)

runparse: parse
	./parse $(GRAMMAR)

runinterp: interp
	./interp $(GRAMMAR)

runextension: extension
	./extension $(GRAMMAR)

clean:
	rm -f parse interp extension
