PROGRAM=_tmancli
CC=gcc
SRCS=$(wildcard lib/*.c cli/*.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))
CFLAGS=-I lib -I cli -Wall

all: build
.PHONY: build clean $(PROGRAM) release debug

build: $(PROGRAM)

release: style check $(PROGRAM)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -rf $(PROGRAM) $(OBJS)

check:
	clear
	cppcheck --std=c89 --enable=all --language=c cli/*.c lib/*.c

valgrind:
	valgrind  --track-origins=yes --leak-check=full --show-leak-kinds=all ./_tmancli

lnum:
	find lib -name '*.c' |  xargs wc -l

install:
	cp _tmancli $HOME/.local/bin

style:
	find . -name '*.[ch]' | xargs indent -nut -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4 -cli0 -d0 -di1 -nfc1 -i4 -ip0 -l80 -lp -npcs -nprs -npsl -sai -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1
	find . -name '*.[ch]\~' | xargs	rm



#build:
#	#gcc -g -Wall lib/*.h lib/*.c -o tman
#	gcc -g  lib/*.h lib/*.c -o _tmancli
#
#prof:
#	# clean up
#	rm -rf tman_prof gmon.out prof.txt
#	gcc -g -Wall -pg lib/*.h lib/*.c -o tman_prof
#	./tman_prof
#	gprof ./tman_prof > prof.txt
#
#
