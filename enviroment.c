#include "enviroment.h"
#include "bignum.h"
#include "constant.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

struct BigNum POWs[42] = {
    "0000000000000000000001", "0000000000000000000003", "0000000000000000000009", "0000000000000000000027", "0000000000000000000081",
    "0000000000000000000243", "0000000000000000000729", "0000000000000000002187", "0000000000000000006561", "0000000000000000019683",
    "0000000000000000059049", "0000000000000000177147", "0000000000000000531441", "0000000000000001594323", "0000000000000004782969",
    "0000000000000014348907", "0000000000000043046721", "0000000000000129140163", "0000000000000387420489", "0000000000001162261467",
    "0000000000003486784401", "0000000000010460353203", "0000000000031381059609", "0000000000094143178827", "0000000000282429536481",
    "0000000000847288609443", "0000000002541865828329", "0000000007625597484987", "0000000022876792454961", "0000000068630377364883",
    "0000000205891132094649", "0000000617673396283947", "0000001853020188851841", "0000005559060566555523", "0000016677181699666569",
    "0000050031545098999707", "0000150094635296999121", "0000450283905890997363", "0001350851717672992089", "0004052555153018976267",
    "0012157665459056928801", "0036472996377170786403"
};

/*
    Reset the game, clear the chessboard.

        Args:
                - short *board (array's start address): chessboard's status

    Results:
        - None, set all blocks on the chessboard to zero.
*/
void reset(short* board)
{
    for (short i = 0; i < (ROW_NUM * COL_NUM); i++)
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
    for (short i = 0; i < COL_NUM; i++) {
        printf("%d ", i);
    }
    printf("\n");
    for (short i = (ROW_NUM * COL_NUM - 1); i >= 0; i--) {
        if (board[i] == BOT_SYMBOL) {
            printf("● ");
        } else if (board[i] == OPPONENT_SYMBOL) {
            printf("◴ ");
        } else {
            printf("◌ ");
        }
        if (i % COL_NUM == 0) {
            printf("\n");
        }
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
    for (int i = 0; i < COL_NUM; i++)
        if (board[(ROW_NUM * COL_NUM - 1) - i] == 0)
            result[index++] = i;
    *length = index;
}

/*
    Get value in the board with validation.

    Args:
        - short *board (array's start pointer): chessboard's status
        - short row (integer): loc's row number
        - short col (integer): loc's col number

    Results:
        - short value (integer): means the value in chessboard[row][col]
*/
short get_loc_status(short* board, short row, short col)
{
    if ((row >= ROW_NUM) || (row < 0)) {
        return -1;
    }
    if ((col >= COL_NUM) || (col < 0)) {
        return -1;
    }
    return board[row * COL_NUM + col];
}

/*
        Return winner's number;

    Args:
        - short *board (array's address): chessboard's status

        Results:
                - short winner_number(integer): winner's number, 0 for no winner now, 1 for Bot, 2 for opponent

    board's coodinate diagram
                                ^
                                | 5
                                | 4
                                | 3
                                | 2
                                | 1
                                | 0
    <-----------------------------
      6   5   4   3   2   1   0 |
*/
short get_winner(short* board)
{
    short a, b, c, d;
    for (short i = 0; i < ROW_NUM; i++) {
        for (short j = 0; j < COL_NUM; j++) {
            // horizontal
            a = get_loc_status(board, i, j);
            b = get_loc_status(board, i, j + 1);
            c = get_loc_status(board, i, j + 2);
            d = get_loc_status(board, i, j + 3);
            if ((a == b) && (b == c) && (c == d) && (a != 0)) {
                return a;
            }

            // vertical
            a = get_loc_status(board, i, j);
            b = get_loc_status(board, i + 1, j);
            c = get_loc_status(board, i + 2, j);
            d = get_loc_status(board, i + 3, j);
            if ((a == b) && (b == c) && (c == d) && (a != 0)) {
                return a;
            }

            // slash (/)
            a = get_loc_status(board, i, j);
            b = get_loc_status(board, i + 1, j - 1);
            c = get_loc_status(board, i + 2, j - 2);
            d = get_loc_status(board, i + 3, j - 3);
            if ((a == b) && (b == c) && (c == d) && (a != 0)) {
                return a;
            }

            // backslash (\)
            a = get_loc_status(board, i, j);
            b = get_loc_status(board, i + 1, j + 1);
            c = get_loc_status(board, i + 2, j + 2);
            d = get_loc_status(board, i + 3, j + 3);
            if ((a == b) && (b == c) && (c == d) && (a != 0)) {
                return a;
            }
        }
    }
    return 0;
}

/*
    Hash chesstable's status into hash.

        Args:
                - short *board (array's address): chessboard's status
        - char *hash (a string): size is BIGNUM_LEN, the hash will be wrote here

        Results:
                - None.
*/
void state_hash(short* board, char* hash)
{
    struct BigNum sum, temp;
    for (short i = 0; i < BIGNUM_LEN; i++) {
        sum.num[i] = '0';
    }

    for (short i = 0; i < (ROW_NUM * COL_NUM); i++) {
        // printf("MUL:\n");
        // printf("%s\n", POWs[i].num);
        temp = mul(POWs[i], board[i]);
        // printf("%s\n\n", temp.num);

        // printf("ADD:\n");
        // printf("%s\n", sum.num);
        // printf("%s\n", temp.num);
        sum = add(sum, temp);
        // printf("%s\n\n", sum.num);
    }

    for (int i = 0; i < BIGNUM_LEN; i++) {
        hash[i] = sum.num[i];
    }
}

/*
    Fall the chess on the board.

    Args:
        - short *board: chessboard
        - struct action *a (struct pointer): action's loc & player

    Results:
        - None. Fall chess on the chessboard
*/
void fall(short* board, struct action* a)
{
    short* ptr = (board + ROW_NUM * COL_NUM - 1 - (a->loc));
    while ((*ptr == 0) && (ptr >= board)) {
        // printf("%d ", *ptr);
        ptr -= COL_NUM;
    }
    *(ptr + COL_NUM) = a->player;
}

/*
    Act on the chessboard.

        Args:
                - short *board (array's address): chessboards' status
                - struct action *a (a action's pointer): include player & choose loc
                - char *state (a string): for return. To save the chessboard's state hash which after doing this action
                - float *reward (pointer): for return. To save the number of rewards which the player gets after doing this action.
                - float *opponent_reward (pointer): for return. To save the number of rewards which the opponents gets after the player doing this action.
                - short *winner (pointer): for return. To save the winner in this action. If haven't finish, it will be zero.

    Results:
        - None. Save in state & reward & winner
*/
void act(short* board, struct action* a, char* state, float* reward, float* opponent_reward, short* winner)
{
    // printf("Act( player=%d, action=%d )\n", a->player, a->loc);
    assert(board[(ROW_NUM * COL_NUM - 1) - (a->loc)] == 0);

    fall(board, a);
    *winner = get_winner(board);
    state_hash(board, state);
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
