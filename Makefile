all: a.out
a.out: main.c
	gcc main.c enviroment.c q-learning.c -lm
run:
	./a.out
