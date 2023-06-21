#include "constant.h"
#include "enviroment.h"
#include "q-learning.h"
#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char* argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // For mpi stop send/receive message
    const char stop[BIGNUM_LEN] = "STOP";
    float empty_value[ACTION_NUM] = {};

    short board[ROW_NUM][COL_NUM] = { 0 };
    short winner;
    struct Node** map; // pointer to pointer, hash table
    bool find;
    float state[ACTION_NUM];

    srand(rank + time(NULL));

    // init hash table
    map = malloc(TABLE_SIZE * sizeof(struct Node*));
    for (int i = 0; i < TABLE_SIZE; i++) {
        map[i] = NULL;
    }

    if (rank == MPI_MASTER) {
        run(map, &board[0][0], false, 10000, false);
    }

    run(map, &board[0][0], true, EPISODE_NUM / size, false);
    // Merge the map
    if (rank == MPI_MASTER) {
        char key[BIGNUM_LEN];
        float value[ACTION_NUM];
        unsigned char stop_count = size - 1;
        while (1) {
            MPI_Recv(key, BIGNUM_LEN, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(value, ACTION_NUM, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (strcmp(key, stop) == 0) {
                stop_count--;
                if (stop_count <= 0) {
                    break;
                }
            } else {
                merge(map, key, value);
            }
        }
    } else {
        MPI_Request request;
        struct Node* temp;
        for (int i = 0; i < TABLE_SIZE; i++) {
            temp = map[i];
            while (temp) {
                MPI_Isend(temp->key, BIGNUM_LEN, MPI_CHAR, MPI_MASTER, 0, MPI_COMM_WORLD, &request);
                MPI_Isend(temp->value, ACTION_NUM, MPI_FLOAT, MPI_MASTER, 0, MPI_COMM_WORLD, &request);
                temp = temp->next;
            }
        }
        MPI_Isend(stop, BIGNUM_LEN, MPI_CHAR, MPI_MASTER, 0, MPI_COMM_WORLD, &request);
        MPI_Isend(empty_value, ACTION_NUM, MPI_FLOAT, MPI_MASTER, 0, MPI_COMM_WORLD, &request);
    }

    if (rank == MPI_MASTER) {
        run(map, &board[0][0], false, 10000, true);
    }

    MPI_Finalize();
}
