
build:
	#gcc -g -Wall src/*.h src/*.c -o tman
	gcc -g  src/*.h src/*.c -o _tman

prof:
	# clean up
	rm -rf tman_prof gmon.out prof.txt
	gcc -g -Wall -pg src/*.h src/*.c -o tman_prof
	./tman_prof
	gprof ./tman_prof > prof.txt

chk:
	clear
	cppcheck --std=c89 --enable=all --language=c src/*.c
