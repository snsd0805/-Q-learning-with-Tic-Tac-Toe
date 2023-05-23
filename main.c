#include <stdio.h>
#define BOT_SYMBOL 1
#define OPPONENT_SYMBOL 2

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
	*length = (index-1);
}

int main(){
	short board[9]= {0};			// tic tac toe's chessboard
	short available_actions[9];
	short available_actions_length;
	
	board[0] = 1;
	board[2] = 2;
	show(board);
	get_available_actions(board, available_actions, &available_actions_length);
	for (short i=0; i<available_actions_length; i++){
		printf("%d ", available_actions[i]);
	}
	printf("\n");
	// reset(board);
	// show(board);
}
