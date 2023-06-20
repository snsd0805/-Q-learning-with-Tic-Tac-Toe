#include "constant.h"
#include "enviroment.h"
#include "q-learning.h"
#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

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
        bool node_exist[1];
        char key[BIGNUM_LEN];
        float value[ACTION_NUM];
        for (int i = 0; i < TABLE_SIZE; i++) {
            for (int j = MPI_MASTER + 1; j < size; j++) {
                while (1) {
                    MPI_Recv(node_exist, 1, MPI_C_BOOL, j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    if (!node_exist[0])
                        break;
                    MPI_Recv(key, BIGNUM_LEN, MPI_CHAR, j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(value, ACTION_NUM, MPI_FLOAT, j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    merge(map, i, key, value);
                }
            }
        }
    } else {
        struct Node* temp;
        bool node_exist[1];
        for (int i = 0; i < TABLE_SIZE; i++) {
            temp = map[i];
            node_exist[0] = true;
            while (temp) {
                MPI_Send(node_exist, 1, MPI_C_BOOL, MPI_MASTER, 0, MPI_COMM_WORLD);
                MPI_Send(temp->key, BIGNUM_LEN, MPI_CHAR, MPI_MASTER, 0, MPI_COMM_WORLD);
                MPI_Send(temp->value, ACTION_NUM, MPI_FLOAT, MPI_MASTER, 0, MPI_COMM_WORLD);
                temp = temp->next;
            }
            node_exist[0] = false;
            MPI_Send(node_exist, 1, MPI_C_BOOL, MPI_MASTER, 0, MPI_COMM_WORLD);
        }
    }

    if (rank == MPI_MASTER) {
        run(map, &board[0][0], false, 10000, true);
    }

    MPI_Finalize();
}
