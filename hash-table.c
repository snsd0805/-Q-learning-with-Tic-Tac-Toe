#include "hash-table.h"
#include "constant.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

long long hash_function(char* key)
{
    long long hash = 0;
    for (int i = 0; i < strlen(key); i++) {
        hash = ((hash * 33) + key[i]) % TABLE_SIZE;
    }
    return hash;
}

void insert(struct Node** table, char* key)
{
    long long hash = hash_function(key);

    struct Node* node = malloc(sizeof(struct Node));
    struct Node *temp, *past;
    strcpy(node->key, key);
    // init
    for (short i = 0; i < ACTION_NUM; i++) {
        node->value[i] = 0.0;
    }
    node->next = NULL;

    if (table[hash] == NULL) {
        table[hash] = node;
    } else {
        temp = table[hash];
        past = NULL;
        while (temp != NULL) {
            assert(strcmp(temp->key, key) != 0);
            past = temp;
            temp = temp->next;
        }
        past->next = node;
    }
}

void search(struct Node** table, char* key, bool* find, float* ans)
{
    long long hash = hash_function(key);
    struct Node *temp, *past;
    *find = false;

    if (table[hash] != NULL) {
        temp = table[hash];
        past = NULL;

        while (temp != NULL) {
            if (strcmp(temp->key, key) == 0) {
                *find = true;
                for (short i = 0; i < ACTION_NUM; i++) {
                    ans[i] = temp->value[i];
                }
                break;
            }
            past = temp;
            temp = temp->next;
        }
    }
}

void update(struct Node** table, char* key, short action, float value)
{
    long long hash = hash_function(key);
    struct Node *temp, *past;
    assert(table[hash] != NULL);

    temp = table[hash];
    past = NULL;
    while (temp != NULL) {
        if (strcmp(temp->key, key) == 0) {
            temp->value[action] = value;
            break;
        }
        past = temp;
        temp = temp->next;
    }
}
