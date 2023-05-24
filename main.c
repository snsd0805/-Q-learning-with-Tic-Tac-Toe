#include <stdio.h>
#include <math.h>
#define BOT_SYMBOL 1
#define OPPONENT_SYMBOL 2

short PATHS[8][3] = {
	{0, 1, 2}, {3, 4, 5}, {6, 7, 8},
	{0, 3, 6}, {1, 4, 7}, {2, 5, 8},
	{0, 4, 8}, {2, 4, 6}
};

void reset(short* board){
/*
	Reset the game, clear the chessboard.
	
	Args:
		- short *board (array's address): chessboard's status
	
	Results:
		- None, set all blocks on the chessboard to zero. 
*/
	for (short i=0; i<9; i++)
		board[i] = 0;
}

void show(short *board){
/*
	Print the chessboard on the console.

	Args:
		- short *board (array's address): chessboard's status
	
	Results:
		- None. Only printing.
*/
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

void get_available_actions(short *board, short *result, short *length){
/*
	Save all available actions into the "result" array.

	Args:
		- short *board (array's address): chessboard's status
		- short *result (array's address): To save all available actions.
		- short *length (integer's pointer): To save the number of available actions.
	
	Results:
		- None. All available actions are saved into "result" and the number of actions is saved in "length"
*/
	short index = 0;	
	for (int i=0; i<9; i++)
		if (board[i] == 0)
			result[index++] = i;
	*length = index;
}

short get_winner(short *board){
/*
	Return winner's number;

	Args:
		- short *board (array's address): chessboard's status

	Results:
		- short winner_number(integer): winner's number, 0 for no winner now, 1 for Bot, 2 for opponent
*/
	int a, b, c;
	for (int i=0; i<8; i++){
		a = PATHS[i][0]; b = PATHS[i][1]; c = PATHS[i][2];
		if ((board[a] == board[b]) && (board[b] == board[c]) && (board[a] != 0)){
			return board[a];
		} 
	}
	return 0;
}

int state_hash(short *board){
/*
	Hash chesstable's status into hash.
	
	Args:
		- short *board (array's address): chessboard's status

	Results:
		- int hash (integer): chessboard's status in i-th block * pow(3, i)
*/
	int base, hash = 0;
	for (int i=0; i<9; i++){
		base = pow(3, i);
		if (board[i]!=0){
			printf("%d * %d + ", base, board[i]);
		}
		hash += (base * board[i]);
	}
	return hash;
}

// void action(short *board, short a, )


int main(){
	short board[9]= {0};			// tic tac toe's chessboard
	short available_actions[9];
	short available_actions_length;
	short winner;
	
	board[1] = 2;
	board[3] = 1;
	board[4] = 1;
	// board[5] = 2;
	// board[7] = 2;

	show(board);
	winner = get_winner(board);
	printf("winner: %d\n", winner);
	
	get_available_actions(board, available_actions, &available_actions_length);
	for (short i=0; i<available_actions_length; i++){
		printf("%d ", available_actions[i]);
	}
	printf("\n");

	int hash = state_hash(board);
	printf("hash: %d", hash);
	// reset(board);
	// show(board);
}
