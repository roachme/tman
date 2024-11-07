
build:
	#gcc -g -Wall src/*.h src/*.c -o tman
	gcc -g  src/*.h src/*.c -o tman

prof:
	gcc -g -Wall -pg src/*.h src/*.c -o tman_prof

chk:
	clear
	cppcheck --std=c89 --enable=all --language=c src/*.c
