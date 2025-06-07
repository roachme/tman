PROGRAM=_tmancli
README=README.md
CC=gcc
SRCS=$(wildcard lib/*.c cli/*.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))
CFLAGS=-I lib -I cli -Wall
LFLAGS=-lconfig

all: build
.PHONY: build clean debug generate $(PROGRAM) release

build: $(PROGRAM)

release: style check $(PROGRAM) generate

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LFLAGS) -DVERSION=\"$(shell cat VERSION.txt)\"

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(LFLAGS)

generate: $(PROGRAM)
	$(shell m4 ./scripts/genreadme > $(README))

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
