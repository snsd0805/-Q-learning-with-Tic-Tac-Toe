#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h>
#include <assert.h>

#define BOT_SYMBOL 1
#define OPPONENT_SYMBOL 2

#define EPSILON 0.9						// Epsilon-greedy
#define LR 0.1							// learning rate
#define LAMBDA 0.9						// discount factor

#define STATE_NUM 19683
#define ACTION_NUM 9
#define EPISODE_NUM 100000
#define FIRST true

short PATHS[8][3] = {
	{0, 1, 2}, {3, 4, 5}, {6, 7, 8},
	{0, 3, 6}, {1, 4, 7}, {2, 5, 8},
	{0, 4, 8}, {2, 4, 6}
};

struct action{
	short player;
	short loc;
};

/*
	Reset the game, clear the chessboard.

	Args:
		- short *board (array's address): chessboard's status

	Results:
		- None, set all blocks on the chessboard to zero.
*/
void reset(short* board){
	for (short i=0; i<9; i++)
		board[i] = 0;
}

/*
	Print the chessboard on the console.

	Args:
		- short *board (array's address): chessboard's status

	Results:
		- None. Only printing.
*/
void show(short *board){
	short loc;
	printf("┼───┼───┼───┼\n");
	for (short i=0; i<3; i++){
		printf("│ ");
		for (short j=0; j<3; j++){
			loc = 3*i+j;
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
void get_available_actions(short *board, short *result, short *length){
	short index = 0;
	for (int i=0; i<9; i++)
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
short get_winner(short *board){
	int a, b, c;
	for (int i=0; i<8; i++){
		a = PATHS[i][0]; b = PATHS[i][1]; c = PATHS[i][2];
		if ((board[a] == board[b]) && (board[b] == board[c]) && (board[a] != 0)){
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
int state_hash(short *board){
	int base, hash = 0;
	for (int i=0; i<9; i++){
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
void act(short *board, struct action *a, int *state, float *reward, float *opponent_reward, short *winner){
    // printf("Act( player=%d, action=%d )\n", a->player, a->loc);
    assert(board[a->loc] == 0);
	board[a->loc] = a->player;
	*winner = get_winner(board);
	*state = state_hash(board);
	if (*winner == a->player){
		*reward = 1.0;
        *opponent_reward = -1.0;
    }
	else if(*winner != 0){
		*reward = -1.0;
        *opponent_reward = 1.0;
    }
	else{
		*reward = 0;
        *opponent_reward = 0;
    }
}

/*
	Return the index with the max value in the array

	Args:
		- short *arr (array's address)
		- short length (integer): array's length

	Results:
		- short index (integer): the index with the max value
*/
short short_argmax(short *arr, short length){
	short ans = -1, max = SHRT_MIN;
	for (short i=0; i<length; i++){
		if (arr[i] > max){
			max = arr[i];
			ans = i;
		}
	}
	return ans;
}

/*
	Return the index with the max value in the array

	Args:
		- float *arr (array's address)
		- short length (integer): array's length

	Results:
		- short index (integer): the index with the max value
*/
short float_argmax(float *arr, short length){
	float ans = -1, max = -FLT_MAX;
	for (short i=0; i<length; i++){
		if (arr[i] > max){
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
		- int state (integer, state hash): hash for board's status

	Results:
		- short best_choice
*/
short bot_choose_action(float *table, short *board, int state){

	// get available actions for choosing
	short available_actions[9];
	short available_actions_length;
	get_available_actions(board, available_actions, &available_actions_length);

	// use argmax() to find the best choise,
	// first we should build an available_actions_state array for saving the state for all available choise.
	float available_actions_state[9];
	short available_actions_state_index[9];
	short available_actions_state_length, index = 0;
	short temp_index, best_choice;
	bool zeros = true;
	for (short i=0; i<available_actions_length; i++){
		temp_index = available_actions[i];
		available_actions_state[index] = *(table + state * ACTION_NUM + temp_index);
		if (available_actions_state[index] != 0.0){
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
	double random_num = (double) rand() / (RAND_MAX + 1.0);
	if ((random_num > EPSILON) || zeros){
		best_choice = available_actions_state_index[ rand() % index ];
	}

    return best_choice;
}

/*
    Opponent random choose a action to do.

    Args:
		- short *table (array's address): state table for Q-Learning
		- short *board (array's address): chessboards' status
		- int state (integer, state hash): hash for board's status

	Results:
		- short choice (integer): random, -1 means no available action to choose
*/
short opponent_random_action(float *table, short *board, int state){

    // get available actions for choosing
    short available_actions[9];
    short available_action_length;
    get_available_actions(board, available_actions, &available_action_length);

    if (available_action_length == 0){
        return -1;
    }

    // random
    short choice;
    choice = (short)( rand() % available_action_length );
    choice = available_actions[choice];

    return choice;
}

/*
    Inilialize the Q-Table

    Args:
        - float *table (two-dim array's start address)

    Results:
        - None.
*/
void init_table(float *table){
    for (int i=0; i<STATE_NUM; i++){
        for (int j=0; j<ACTION_NUM; j++){
            *(table + i * ACTION_NUM + j) = 0;
        }
    }
}

/*
    Give the chessboard & state, it will return the max reward with the best choice

    Args:
        - float *table (2-dim array's start address)
        - short *board (1-dim array's start address): chessboard's address
        - int state (integer): board state's hash

    Results:
        - int max_reward
*/
float get_estimate_reward(float *table, short *board, int state){
    short available_actions[9];
    short available_action_length;
    get_available_actions(board, available_actions, &available_action_length);

	float available_actions_state[9];
    for (short i=0; i<available_action_length; i++){
        available_actions_state[i] = *(table + state * ACTION_NUM + available_actions[i]);   // table[state][available_actions[i]]
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
void run(float *table, short *board, bool train, int times, bool plot){
	short available_actions[9];
	short available_actions_length;
	short winner;
    short choice, opponent_choice;
    int state, _state;
    float estimate_r, estimate_r_, real_r, r, opponent_r;
    struct action a;

    int win = 0;

    for (int episode=0; episode<times; episode++){
        reset(board);
        state = state_hash(board);
        while (1){
            // bot choose the action
            choice = bot_choose_action(table, board, state);
            a.loc = choice;
            a.player = BOT_SYMBOL;

            estimate_r = *(table + state * ACTION_NUM + choice);
            act(board, &a, &_state, &r, &opponent_r, &winner);
            if (plot) show(board);

            // opponent random
            if (winner == 0){
                opponent_choice = opponent_random_action(table, board, state_hash(board));
                if (opponent_choice != -1){
                    a.loc = opponent_choice;
                    a.player = OPPONENT_SYMBOL;
                    act(board, &a, &_state, &opponent_r, &r, &winner);
                    if (plot) show(board);
                }
            }
            get_available_actions(board, available_actions, &available_actions_length);

            if ((winner != 0) || (available_actions_length == 0)){
                if (plot){
                    printf("winner: %d, reward: %f, oppo reward: %f\n", winner, r, opponent_r);
                    printf("==========================================================\n");
                }
                real_r = r;
            } else {
                estimate_r_ = get_estimate_reward(table, board, _state);
                real_r = r + LAMBDA * estimate_r_;
            }
            if (train){
                // printf("update");
                *(table + state * ACTION_NUM + choice) += ( LR * (real_r - estimate_r) );       // table[state][choice] += LR * (real_r - estimate_r)
            }
            state = _state;

            if ((winner != 0) || (available_actions_length == 0)){
                // printf("break\n");
                if (winner == 1){
                    win += 1;
                }
                break;
            }
        }
    }

    if (!train)
        printf("%d/%d, %f\%\n", win, 10000, (float)win/10000);
}

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
    run(&table[0][0], board, true, 10000000, false);
    run(&table[0][0], board, false, 10000, false);

}
