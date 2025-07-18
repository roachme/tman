MAKE=make
PROGRAM=_tmancli
VERSION=$(shell cat VERSION.txt)
README=README.md
SHELLSCRIPT=tman.sh
SHELLNAME=bash
CC=gcc
SRCS=$(wildcard cli/*.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))
CFLAGS=-I lib/src -I cli -Wall -g3  -fbounds-check
LFLAGS=-lconfig -g
PWDFILE=/tmp/tmanpwd

# TODO: add debug mode
# TODO: add user and developer builds

all: $(PROGRAM)
.PHONY: clean debug generate release

init:
	mkdir -p artifacts
	mkdir -p artifacts/build/debug
	mkdir -p artifacts/build/release
	mkdir -p artifacts/docs
	mkdir -p artifacts/reports
	mkdir -p artifacts/cppcheck
	mkdir -p artifacts/doxygen
	mkdir -p artifacts/flawfinder
	mkdir -p artifacts/ldoc
	mkdir -p artifacts/luacheck
	#ln -s ../build/artifacts/gcov artifacts/gcov ||:
	#ln -s ../build/gtest artifacts/gtest ||:

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) -DVERSION=\"$(VERSION)\" -DPWDFILE=\"$(PWDFILE)\"

$(PROGRAM): $(OBJS)
	$(MAKE) -C lib lib_static
	$(CC) -o $@ $^ $(LFLAGS) lib/libtman.a

generate: $(PROGRAM)
	$(shell m4 -DSHELLNAME=$(SHELLNAME) -DPWDFILE=$(PWDFILE) ./scripts/genshell > $(SHELLSCRIPT))
	$(shell m4 ./scripts/genreadme > $(README))

clean:
	rm -rf artifacts build
	rm -rf $(PROGRAM) $(OBJS)
	$(MAKE) -C lib lib_clean

check:
	cppcheck --std=c89 --enable=all --language=c $(SRCS)

valgrind:
	valgrind  --track-origins=yes --leak-check=full --show-leak-kinds=all ./$(PROGRAM)

lnum:
	find lib -name '*.c' | xargs wc -l

install:
	cp $(PROGRAM) $HOME/.local/bin

style:
	find . -name '*.[ch]' | xargs indent -nut -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4 -cli0 -d0 -di1 -nfc1 -i4 -ip0 -l80 -lp -npcs -nprs -npsl -sai -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1
	find . -name '*.[ch]\~' | xargs	rm

# Build project in debug configuration into ./artifacts/build/debug
.PHONY: build_debug
build_debug:
	make debug


# Build project in release configuraiton into ./artifacts/build/release
.PHONY: build_release
build_release:
	make


#all_release: clean init check_style test_unit test_integration test_system test_e2e check_static check_security build_release docs_build ;
release: clean init style check $(PROGRAM) generate


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
