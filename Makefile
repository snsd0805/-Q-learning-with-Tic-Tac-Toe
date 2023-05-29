all: a.out
a.out: main.c enviroment.c enviroment.h q-learning.c q-learning.h constant.h
	gcc main.c enviroment.c q-learning.c -lm
run:
	./a.out
