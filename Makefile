PROGRAM=_tman
CC=gcc
SRCS=$(wildcard src/*.c src/bin/*.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))
CFLAGS=-I src

.PHONY: clean $(PROGRAM)



%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -rf $(PROGRAM) $(OBJS)

chk:
	clear
	cppcheck --std=c89 --enable=all --language=c src/bin/*.c src/core/*.c src/pgn/*.c

#build:
#	#gcc -g -Wall src/*.h src/*.c -o tman
#	gcc -g  src/*.h src/*.c -o _tman
#
#prof:
#	# clean up
#	rm -rf tman_prof gmon.out prof.txt
#	gcc -g -Wall -pg src/*.h src/*.c -o tman_prof
#	./tman_prof
#	gprof ./tman_prof > prof.txt
#
#
