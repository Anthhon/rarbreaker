#ifndef HASH_SET_H
#define HASH_SET_H

#define HASH_SET_SIZE 100

typedef struct Node {
    char *key;
    struct Node *next;
} Node;

typedef struct {
    Node *buckets[HASH_SET_SIZE];
} HashSet;

unsigned int hash_function(const char *key);
Node* create_node(const char *key);
void hash_set_initialize(HashSet *set);
void set_insert(HashSet *set, const char *key);
bool set_contains(HashSet *set, const char *key);
void clean_hash_set(HashSet *set);
void free_hash_set(HashSet *set);

#endif // HASH_SET_H
