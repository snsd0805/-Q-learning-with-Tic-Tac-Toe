#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include "constant.h"
#include "enviroment.h"
#include "q-learning.h"

int main(){
	short board[9]= {0};			        // tic tac toe's chessboard
    float table[STATE_NUM][ACTION_NUM];     // q-learning table

	srand(time(NULL));
    init_table(&table[0][0]);

    run(&table[0][0], board, false, 10000, false);
    run(&table[0][0], board, true, 1000000, false);
    run(&table[0][0], board, false, 10000, false);
}
