short float_argmax(float* arr, short length);
short bot_choose_action(float* table, short* board, int state);
short opponent_random_action(float* table, short* board, int state);
void init_table(float* table);
float get_estimate_reward(float* table, short* board, int state);
void run(float* table, short* board, bool train, int times, bool plot);