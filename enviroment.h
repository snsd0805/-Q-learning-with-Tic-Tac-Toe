struct action{
	short player;
	short loc;
};

void reset(short* board);
void show(short *board);
void get_available_actions(short *board, short *result, short *length);
short get_winner(short *board);
void state_hash(short *board, char *hash);
void act(short *board, struct action *a, char *state, float *reward, float *opponent_reward, short *winner);
