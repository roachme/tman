PROGRAM=_tman
CC=gcc
SRCS=$(wildcard *.c)
SRCS2=$(wildcard *.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))
CFLAGS=-I lib -I cli

all: $(PROGRAM)
.PHONY: clean $(PROGRAM)

include ../lib/Makefile


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(PROGRAM): $(OBJS) $(LIBSTATIC)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean-cli:
	rm -rf $(PROGRAM) $(OBJS)
	pwd
	@echo "SRCS: " $(SRCS)
	@echo "SRCS2: " $(SRCS2)
	@echo "cli/makefile is done"

chk:
	clear
	cppcheck --std=c89 --enable=all --language=c cli/*.c lib/*.c

valgrind:
	valgrind  --track-origins=yes --leak-check=full --show-leak-kinds=all ./_tman

lnum:
	find lib -name '*.c' |  xargs wc -l

test:

