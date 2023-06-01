#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#define TABLE_SIZE 10

struct Node {
    char key[48];
    int value;
    struct Node *next;
};

long long hash_function(char *key) {
    long long hash = 0;
    for (int i=0; i<strlen(key); i++){
        hash = ((hash * 33) + key[i]) % TABLE_SIZE;
    }
    return hash ;
}

void insert(struct Node **table, char *key, int value) {
    long long hash = hash_function(key);

    printf("Hash: %lli\n", hash);

    struct Node *node = malloc(sizeof(struct Node));
    struct Node *temp, *past;
    strcpy(node->key, key);
    node->value = value;
    node->next = NULL;

    if (table[hash] == NULL){
        table[hash] = node;
        printf("Create.\n");
    } else {
        printf("Add.\n");
        temp = table[hash];
        past = NULL;
        while(temp != NULL){
            assert(temp->key != key);
            printf("%s -> ", temp->key);
            past = temp;
            temp = temp->next;
        }
        printf("\n");
        past->next = node;
    }
}

void long_to_str(long long num, char *s, int length) {
    int temp;
    for (int i=length-1; i>=0; i--) {
        temp = num % 10;
        num /= 10;
        s[i] = (char)(temp + 48);
    }
}

int search(struct Node **table, char *key) {
    long long hash = hash_function(key);
    struct Node *temp, *past;

    if (table[hash] == NULL){
        return -1;
    } else {
        temp = table[hash];
        past = NULL;

        while(temp != NULL){
            // printf("%s - %s\n", temp->key, key);
            if (strcmp(temp->key, key) == 0){
                return temp->value;
            }
            past = temp;
            temp = temp->next;
        }
        return -1;
    }
}

void update(struct Node **table, char *key, int value) {
    long long hash = hash_function(key);
    struct Node *temp, *past;

    temp = table[hash];
    past = NULL;
    while(temp != NULL){
        if (strcmp(temp->key, key) == 0){
            temp->value = value;
            break;
        }
        past = temp;
        temp = temp->next;
    }
}

int main(){
    struct Node ** table;      // pointer to pointer
    int size;
    srand(time(NULL));

    table = malloc(TABLE_SIZE * sizeof(struct Node*));
    for (int i=0; i<TABLE_SIZE; i++){
        table[i] = NULL;
    }

    long long a = 1234567890;
    char s[21];
    for (int i=0; i<50; i++){
        a = (long long)rand();
        printf("%lli\n", a);
        long_to_str(a, s, 20);
        printf("%s\n", s);
        insert(table, s, i);
        printf("\n");
    }

    int ans;
    while (1) {
        printf("> ");
        scanf("%lli", &a);
        printf("HERE\n");
        long_to_str(a, s, 20);
        printf("HERE\n");

        update(table, s, 100);
        ans = search(table, s);
        printf("%d\n\n", ans);
    }
    // long long a = hash_function("9999999999999");
    // printf("%lli\n", a);
}