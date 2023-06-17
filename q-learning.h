#include "hash-table.h"

short float_argmax(float* arr, short length);
short bot_choose_action(struct Node** map, short* board, char* state);
short opponent_random_action(short* board);
float get_estimate_reward(struct Node** map, short* board, char* state);
void run(struct Node** map, short* board, bool train, int times, bool plot);
void run_adversarial(struct Node** map, struct Node** oppo_map, short* board, bool train, int times, bool plot);