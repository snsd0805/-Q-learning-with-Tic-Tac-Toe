struct action {
    short player;
    short loc;
};

void reset(short* board);
void show(short* board);
void get_available_actions(short* board, short* result, short* length);
short get_winner(short* board);
int state_hash(short* board);
void act(short* board, struct action* a, int* state, float* reward, float* opponent_reward, short* winner);
