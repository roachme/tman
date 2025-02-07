PROGRAM=_tman
CC=gcc
SRCS=$(wildcard lib/*.c cli/*.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))
CFLAGS=-I lib -I cli

all: $(PROGRAM)
.PHONY: clean $(PROGRAM)


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -rf $(PROGRAM) $(OBJS)

chk:
	clear
	cppcheck --std=c89 --enable=all --language=c cli/*.c lib/*.c

valgrind:
	valgrind  --track-origins=yes --leak-check=full --show-leak-kinds=all ./_tman

lnum:
	find lib -name '*.c' |  xargs wc -l

install:
	cp _tman $HOME/.local/bin

#build:
#	#gcc -g -Wall lib/*.h lib/*.c -o tman
#	gcc -g  lib/*.h lib/*.c -o _tman
#
#prof:
#	# clean up
#	rm -rf tman_prof gmon.out prof.txt
#	gcc -g -Wall -pg lib/*.h lib/*.c -o tman_prof
#	./tman_prof
#	gprof ./tman_prof > prof.txt
#
#
