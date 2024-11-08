
build:
	#gcc -g -Wall src/*.h src/*.c -o tman
	gcc -g  src/*.h src/*.c -o _tman

prof:
	gcc -g -Wall -pg src/*.h src/*.c -o tman_prof
	gprof ./tman_prof > prof.txt

chk:
	clear
	cppcheck --std=c89 --enable=all --language=c src/*.c
