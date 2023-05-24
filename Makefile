all: a.out
a.out: main.c
	gcc main.c -lm
run:
	./a.out
