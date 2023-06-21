#include "hash-table.h"
#include "constant.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
    djb2 hash algorithm for hash table

    Args:
        - char* key (string): State Hash for chessboard, It is a string of size BIGNUM_LEN in this project.

    Results:
        - long long hash: for indexing in the hash table
*/
long long hash_function(char* key)
{
    long long hash = 0;
    for (int i = 0; i < strlen(key); i++) {
        hash = ((hash * 33) + key[i]) % TABLE_SIZE;
    }
    return hash;
}

/*
    Insert a node in the hash table, and initilize the array(value) in the nodes to zeros.

    Args:
        - struct Node** table(Node pointer array): hash table's start address
        - char* key (string): State hash for the chessboard

    Results:
        - None. Only link a new node in the hash table. All value in the node's arrays is zeros.
*/
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

/*
    Get node's array's value in the hash table.

    Args:
        - struct Node** table(Node pointer array): hash table's start address
        - char* key (string): State hash for the chessboard
        - bool* find: whether find this node in the hash table.
        - float* ans: if find this node, this function will put it's value(array) in this variable.

    Results:
        - None
*/
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

/*
    Update node's array's value in the hash table.

    Args:
        - struct Node** table(Node pointer array): hash table's start address
        - char* key (string): State hash for the chessboard
        - short action: which column in this node's array will be changed.
        - float value: target value.

    Results:
        - None
*/
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

/*
    Merge the node into table

    Args:
        - struct Node** table(Node pointer array): hash table's start address
        - struct Node* node: node pointer, containing key and value

    Results:
        - None
*/
void merge(struct Node** table, struct Node* node)
{
    long long hash = hash_function(node->key);
    struct Node* new_node = malloc(sizeof(struct Node));
    struct Node *temp, *past;
    strcpy(new_node->key, node->key);
    // init
    temp = table[hash];
    past = NULL;
    new_node->next = NULL;
    for (short i = 0; i < ACTION_NUM; i++) {
        new_node->value[i] = node->value[i];
    }

    if (temp == NULL) {
        temp = new_node;
    } else {
        while (temp != NULL) {
            // find key in the table
            if (strcmp(temp->key, new_node->key) == 0) {
                for (short i = 0; i < ACTION_NUM; i++) {
                    temp->value[i] += new_node->value[i];
                }
                free(new_node);
                return;
            }
            past = temp;
            temp = temp->next;
        }
        // key is not in the table
        temp = new_node;
    }
}
