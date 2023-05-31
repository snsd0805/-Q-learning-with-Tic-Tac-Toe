#include "enviroment.h"
#include "constant.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

short PATHS[8][3] = {
    { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 },
    { 0, 3, 6 }, { 1, 4, 7 }, { 2, 5, 8 },
    { 0, 4, 8 }, { 2, 4, 6 }
};

/*
    Reset the game, clear the chessboard.

    Args:
        - short *board (array's address): chessboard's status

    Results:
        - None, set all blocks on the chessboard to zero.
*/
void reset(short* board)
{
    for (short i = 0; i < 9; i++)
        board[i] = 0;
}

/*
    Print the chessboard on the console.

    Args:
        - short *board (array's address): chessboard's status

    Results:
        - None. Only printing.
*/
void show(short* board)
{
    short loc;
    printf("┼───┼───┼───┼\n");
    for (short i = 0; i < 3; i++) {
        printf("│ ");
        for (short j = 0; j < 3; j++) {
            loc = 3 * i + j;
            if (board[loc] == 0)
                printf("  │ ");
            else if (board[loc] == BOT_SYMBOL)
                printf("○ │ ");
            else
                printf("✕ │ ");
        }
        printf("\n");
        printf("┼───┼───┼───┼\n");
    }
    printf("\n\n");
}

/*
    Save all available actions into the "result" array.

    Args:
        - short *board (array's address): chessboard's status
        - short *result (array's address): To save all available actions.
        - short *length (integer's pointer): To save the number of available actions.

    Results:
        - None. All available actions are saved into "result" and the number of actions is saved in "length"
*/
void get_available_actions(short* board, short* result, short* length)
{
    short index = 0;
    for (int i = 0; i < 9; i++)
        if (board[i] == 0)
            result[index++] = i;
    *length = index;
}

/*
    Return winner's number;

    Args:
        - short *board (array's address): chessboard's status

    Results:
        - short winner_number(integer): winner's number, 0 for no winner now, 1 for Bot, 2 for opponent
*/
short get_winner(short* board)
{
    int a, b, c;
    for (int i = 0; i < 8; i++) {
        a = PATHS[i][0];
        b = PATHS[i][1];
        c = PATHS[i][2];
        if ((board[a] == board[b]) && (board[b] == board[c]) && (board[a] != 0)) {
            return board[a];
        }
    }
    return 0;
}

/*
    Hash chesstable's status into hash.

    Args:
        - short *board (array's address): chessboard's status

    Results:
        - int hash (integer): chessboard's status in i-th block * pow(3, i)
*/
int state_hash(short* board)
{
    int base, hash = 0;
    for (int i = 0; i < 9; i++) {
        base = pow(3, i);
        hash += (base * board[i]);
    }
    return hash;
}

/*
    Act on the chessboard.

    Args:
        - short *board (array's address): chessboards' status
        - struct action *a (a action's pointer): include player & choose loc
        - int *state (pointer): for return. To save the chessboard's state hash which after doing this action
        - float *reward (pointer): for return. To save the number of rewards which the player gets after doing this action.
        - float *opponent_reward (pointer): for return. To save the number of rewards which the opponents gets after the player doing this action.
        - short *winner (pointer): for return. To save the winner in this action. If haven't finish, it will be zero.

    Results:
        - None. Save in state & reward & winner
*/
void act(short* board, struct action* a, int* state, float* reward, float* opponent_reward, short* winner)
{
    // printf("Act( player=%d, action=%d )\n", a->player, a->loc);
    assert(board[a->loc] == 0);
    board[a->loc] = a->player;
    *winner = get_winner(board);
    *state = state_hash(board);
    if (*winner == a->player) {
        *reward = 1.0;
        *opponent_reward = -1.0;
    } else if (*winner != 0) {
        *reward = -1.0;
        *opponent_reward = 1.0;
    } else {
        *reward = 0;
        *opponent_reward = 0;
    }
}
