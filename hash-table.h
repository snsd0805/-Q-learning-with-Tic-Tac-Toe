#include "constant.h"
#include <stdbool.h>

struct Node {
    char key[BIGNUM_LEN + 1];
    float value[ACTION_NUM];
    struct Node* next;
};

long long hash_function(char* key);
void insert(struct Node** table, char* key);
void search(struct Node** table, char* key, bool* find, float* ans);
void update(struct Node** table, char* key, short action, float value);