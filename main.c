#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include "constant.h"
#include "enviroment.h"
#include "q-learning.h"

int main(){
	short board[ROW_NUM][COL_NUM]= {0};
    short winner;
    struct Node ** map;      // pointer to pointer, hash table
    bool find;
    float state[ACTION_NUM];

    srand(time(NULL));

    // init hash table
    map = malloc(TABLE_SIZE * sizeof(struct Node*));
    for (int i=0; i<TABLE_SIZE; i++){
        map[i] = NULL;
    }

    run(map, &board[0][0], false, 10000, false);
    run(map, &board[0][0], true, EPISODE_NUM, false);
    run(map, &board[0][0], false, 10000, true);
}
