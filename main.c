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
#include <ctype.h>
void test_board(int rank,int size,struct Node** map,short* board);
void train_board(int rank,int size,struct Node** map,short* board);
void play_board(int rank,int size,struct Node** map,short* board);
int main(int argc, char* argv[])
{
    int rank, size;
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
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int command;
    
    if (rank == MPI_MASTER) 
    {
        printf("\nEnter command: \n (1) for test win rate\n (2) for training\n (3) for play\n (4) for stop\n");
        while(scanf("%d",&command))
        {
            MPI_Bcast(&command,1,MPI_INT,0,MPI_COMM_WORLD);
            if (command==1)
                test_board(rank,size,map,&board[0][0]);
            else if (command==2)
                train_board(rank,size,map,&board[0][0]);
            else if (command==3)
                play_board(rank,size,map,&board[0][0]);
            else if(command==4)
                break;
            printf("\nEnter command: \n (1) for test win rate\n (2) for training\n (3) for play\n (4) for stop\n");
        }
    }
    else
    {
        while(1)
        {
            MPI_Bcast(&command,1,MPI_INT,0,MPI_COMM_WORLD);
            if (command==2)
                train_board(rank,size,map,&board[0][0]);
            if (command==4)
                    break;
        }
    }   
    free(map);
    MPI_Finalize();
}
void play_board(int rank,int size,struct Node** map,short* board)
{
    if (rank == MPI_MASTER)
        run(map, board, false, 1, true,true);
}
void train_board(int rank,int size,struct Node** map,short* board)
{

    int blocklen[3] = { BIGNUM_LEN + 1, ACTION_NUM, 0 };
    MPI_Aint disps[3] = { offsetof(struct Node, key), offsetof(struct Node, value), offsetof(struct Node, next) };
    // next is pointer, but there is no data type pointer for MPI
    // data type long should be same as pointer
    MPI_Datatype oldtypes[3] = { MPI_INT, MPI_FLOAT, MPI_LONG };
    MPI_Datatype TYPE_NODE;
    MPI_Type_create_struct(3, blocklen, disps, oldtypes, &TYPE_NODE);
    MPI_Type_commit(&TYPE_NODE);
    struct Node* node;
    run(map, board, true, EPISODE_NUM / size, false,false);
    // Merge the map
    if (rank == MPI_MASTER) {
        printf("Training...\n");
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
        for (int i = 0; i < TABLE_SIZE; i++) {
            node = map[i];
            while (node) {
                MPI_Send(node, 1, TYPE_NODE, MPI_MASTER, 0, MPI_COMM_WORLD);
                node = node->next;
            }
        }
        node = malloc(sizeof(struct Node));
        strcpy(node->key, STOP_MESSAGE);
        MPI_Send(node, 1, TYPE_NODE, MPI_MASTER, 0, MPI_COMM_WORLD);
        free(node);
    }
}

void test_board(int rank,int size,struct Node** map,short* board)
{

    printf("Testing...\n");
    if (rank == MPI_MASTER) {
        run(map, board, false, 10000, false,false);
    }
}
