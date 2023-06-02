#include <float.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "constant.h"
#include "enviroment.h"
#include "hash-table.h"

/*
    Return the index with the max value in the array

    Args:
        - float *arr (array's address)
        - short length (integer): array's length

    Results:
        - short index (integer): the index with the max value
*/
short float_argmax(float* arr, short length)
{
    float ans = -1, max = -FLT_MAX;
    for (short i = 0; i < length; i++) {
        if (arr[i] > max) {
            max = arr[i];
            ans = i;
        }
    }
    return ans;
}

/*
    Choose the next action with Epsilon-Greedy.
    EPSILON means the probability to choose the best action in this state from Q-Table.
    (1-EPSILON) to random an action to do.

    Args:
        - short *table (array's address): state table for Q-Learning
        - short *board (array's address): chessboards' status
        - char *state (string, state hash): hash for board's status

    Results:
        - short best_choice
*/
short bot_choose_action(struct Node** map, short* board, char* state)
{

    // get available actions for choosing
    short available_actions[ACTION_NUM];
    short available_actions_length;
    get_available_actions(board, available_actions, &available_actions_length);

    // use argmax() to find the best choise,
    // first we should build an available_actions_state array for saving the state for all available choise.
    float available_actions_state[ACTION_NUM];
    short available_actions_state_index[ACTION_NUM];
    short available_actions_state_length, index = 0;
    short temp_index, best_choice;
    bool zeros = true;
    bool find;
    float state_weights[ACTION_NUM];

    // find weights in the hash table
    search(map, state, &find, state_weights);
    if (!find) {
        for (short i = 0; i < ACTION_NUM; i++) {
            state_weights[i] = 0.0;
        }
    }

    // get the best choice
    for (short i = 0; i < available_actions_length; i++) {
        temp_index = available_actions[i];

        available_actions_state[index] = state_weights[temp_index];
        if (available_actions_state[index] != 0.0) {
            zeros = false;
        }
        available_actions_state_index[index] = temp_index;
        index++;
    }
    best_choice = float_argmax(available_actions_state, index);
    best_choice = available_actions_state_index[best_choice];

    // Epsilon-Greedy
    // If random number > EPSILON   ->   random a action
    // If random number < EPSILON   ->   choose the best action in this state.
    double random_num = (double)rand() / (RAND_MAX + 1.0);
    if ((random_num > EPSILON) || zeros) {
        best_choice = available_actions_state_index[rand() % index];
    }

    return best_choice;
}

/*
    Opponent random choose a action to do.

    Args:
        - short *board (array's address): chessboards' status

    Results:
        - short choice (integer): random, -1 means no available action to choose
*/
short opponent_random_action(short* board)
{

    // get available actions for choosing
    short available_actions[ACTION_NUM];
    short available_action_length;
    get_available_actions(board, available_actions, &available_action_length);

    if (available_action_length == 0) {
        return -1;
    }

    // random
    short choice;
    choice = (short)(rand() % available_action_length);
    choice = available_actions[choice];

    return choice;
}

//     Use Hash Table, so we needn't initilize Q-Table
//
// /*
//     Inilialize the Q-Table

//     Args:
//         - float *table (two-dim array's start address)

//     Results:
//         - None.
// */
// void init_table(float *table){
//     for (int i=0; i<STATE_NUM; i++){
//         for (int j=0; j<ACTION_NUM; j++){
//             *(table + i * ACTION_NUM + j) = 0;
//         }
//     }
// }

/*
    Give the chessboard & state, it will return the max reward with the best choice

    Args:
        - float *table (2-dim array's start address)
        - short *board (1-dim array's start address): chessboard's address
        - int state (integer): board state's hash

    Results:
        - int max_reward
*/
float get_estimate_reward(struct Node** map, short* board, char* state)
{
    short available_actions[ACTION_NUM];
    short available_action_length;
    get_available_actions(board, available_actions, &available_action_length);

    // find weights in the hash table
    float state_weights[ACTION_NUM];
    bool find;
    search(map, state, &find, state_weights);
    if (!find) {
        for (short i = 0; i < ACTION_NUM; i++) {
            state_weights[i] = 0.0;
        }
    }

    float available_actions_state[ACTION_NUM];
    for (short i = 0; i < available_action_length; i++) {
        available_actions_state[i] = state_weights[available_actions[i]]; // table[state][available_actions[i]]
    }

    short ans_index;
    ans_index = float_argmax(available_actions_state, available_action_length);
    return available_actions_state[ans_index];
}

/*
    Run Q-learning Evaluation or Training.

    Args:
        - float *table (2-dim array's start address)
        - short *board (1-dim array's start address): chessboard's address
        - bool train: train or not
        - int times: how many episode to simulate
        - bool plot: whether to plot the gaming process

    Results:
        - None
*/
void run(struct Node** map, short* board, bool train, int times, bool plot)
{
    short available_actions[ACTION_NUM];
    short available_actions_length;
    short winner;
    short choice, opponent_choice;
    char state[BIGNUM_LEN], _state[BIGNUM_LEN];
    float estimate_r, estimate_r_, real_r, r, opponent_r;
    struct action a;
    float state_weights[ACTION_NUM];
    bool find;
    int win = 0;

    for (int episode = 0; episode < times; episode++) {
        reset(board);
        state_hash(board, state);
        while (1) {
            // bot choose the action
            choice = bot_choose_action(map, board, state);
            a.loc = choice;
            a.player = BOT_SYMBOL;

            search(map, state, &find, state_weights);
            if (!find) {
                for (short i = 0; i < ACTION_NUM; i++) {
                    state_weights[i] = 0.0;
                }
                if (train)
                    insert(map, state);
            }
            estimate_r = state_weights[choice];
            act(board, &a, _state, &r, &opponent_r, &winner);
            if (plot)
                show(board);

            // // opponent random
            if (winner == 0) {
                opponent_choice = opponent_random_action(board);
                if (opponent_choice != -1) {
                    a.loc = opponent_choice;
                    a.player = OPPONENT_SYMBOL;
                    act(board, &a, _state, &opponent_r, &r, &winner);
                    if (plot)
                        show(board);
                }
            }
            get_available_actions(board, available_actions, &available_actions_length);

            if ((winner != 0) || (available_actions_length == 0)) {
                if (plot) {
                    printf("winner: %d, reward: %f, oppo reward: %f\n", winner, r, opponent_r);
                    printf("==========================================================\n");
                }
                real_r = r;
            } else {
                estimate_r_ = get_estimate_reward(map, board, _state);
                real_r = r + LAMBDA * estimate_r_;
            }
            if (train) {
                state_weights[choice] += (LR * (real_r - estimate_r));
                update(map, state, choice, state_weights[choice]);
            }
            for (int i = 0; i < BIGNUM_LEN; i++) {
                state[i] = _state[i];
            }

            if ((winner != 0) || (available_actions_length == 0)) {
                if (winner == 1) {
                    win += 1;
                }
                break;
            }
        }
    }

    if (!train)
        // printf("%d/%d, %f\%\n", win, 10000, (float)win/10000);
        printf("%f\n", (float)win / times);
}
