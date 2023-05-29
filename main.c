#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include "constant.h"
#include "enviroment.h"
#include "q-learning.h"

int main(){
	short board[9]= {0};			// tic tac toe's chessboard
    float table[STATE_NUM][ACTION_NUM];
	short available_actions[9];
	short available_actions_length;
	short winner;
    short choice, opponent_choice;
    int state, _state;
    int estimate_r, estimate_r_, real_r, r, opponent_r;
    struct action a;

	srand(time(NULL));
    init_table(&table[0][0]);

    run(&table[0][0], board, false, 10000, false);
    run(&table[0][0], board, true, 1000000, false);
    run(&table[0][0], board, false, 10000, false);
}
