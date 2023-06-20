all: a.out
a.out: main.c enviroment.c enviroment.h q-learning.c q-learning.h bignum.c bignum.h hash-table.c hash-table.h constant.h
	mpicc main.c enviroment.c q-learning.c bignum.c constant.h hash-table.c -lm
run:
	mpirun -n 4 ./a.out
