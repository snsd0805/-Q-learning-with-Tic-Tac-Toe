#include "constant.h"
#include "enviroment.h"
#include "q-learning.h"
#include <mpi.h>
#include <stdbool.h>
#include <stddef.h>
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

    int blocklen[3] = { BIGNUM_LEN + 1, ACTION_NUM, 0 };
    MPI_Aint disps[3] = { offsetof(struct Node, key), offsetof(struct Node, value), offsetof(struct Node, next) };
    // next is pointer, but there is no data type pointer for MPI
    // data type long should be same as pointer
    MPI_Datatype oldtypes[3] = { MPI_INT, MPI_FLOAT, MPI_LONG };
    MPI_Datatype TYPE_NODE;
    MPI_Type_create_struct(3, blocklen, disps, oldtypes, &TYPE_NODE);
    MPI_Type_commit(&TYPE_NODE);

    short board[ROW_NUM][COL_NUM] = { 0 };
    short winner;
    struct Node** map; // pointer to pointer, hash table
    struct Node* node;
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
        node = malloc(sizeof(struct Node));
        unsigned char stop_count = size - 1;
        while (1) {
            MPI_Recv(node, 1, TYPE_NODE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (strcmp(node->key, STOP_MESSAGE) == 0) {
                stop_count--;
                if (stop_count <= 0) {
                    break;
                }
            } else {
                merge(map, node);
            }
        }
        free(node);
    } else {
        MPI_Request request;
        // struct Node* temp;
        for (int i = 0; i < TABLE_SIZE; i++) {
            node = map[i];
            while (node) {
                MPI_Isend(node, 1, TYPE_NODE, MPI_MASTER, 0, MPI_COMM_WORLD, &request);
                node = node->next;
            }
        }
        node = malloc(sizeof(struct Node));
        strcpy(node->key, STOP_MESSAGE);
        MPI_Isend(node, 1, TYPE_NODE, MPI_MASTER, 0, MPI_COMM_WORLD, &request);
        free(node);
    }

    if (rank == MPI_MASTER) {
        run(map, &board[0][0], false, 10000, true);
    }

    MPI_Finalize();
}
