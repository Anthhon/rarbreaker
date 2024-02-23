#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "hash_set.h"

unsigned int hash_function(const char *key)
{
    unsigned int hash = 0;
    while (*key) {
        hash = (hash * 31) + *key++;
    }
    return hash % HASH_SET_SIZE;
}

Node* create_node(const char *key)
{
    Node *node = malloc(sizeof(Node));
    if (node != NULL) {
        node->key = strdup(key);
        node->next = NULL;
    }
    return node;
}

void hash_set_initialize(HashSet *set)
{
    for (int i = 0; i < HASH_SET_SIZE; ++i) {
        set->buckets[i] = NULL;
    }
}

void set_insert(HashSet *set, const char *key)
{
    unsigned int index = hash_function(key);
    Node *node = create_node(key);
    if (node == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    node->next = set->buckets[index];
    set->buckets[index] = node;
}

bool set_contains(HashSet *set, const char *key)
{
    unsigned int index = hash_function(key);
    Node *current = set->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return true;
        }
        current = current->next;
    }
    return false;
}

void clean_hash_set(HashSet *set)
{
    for (int i = 0; i < HASH_SET_SIZE; ++i) {
        Node *current = set->buckets[i];
        while (current != NULL) {
            Node *temp = current;
            current = current->next;
            free(temp->key);
            free(temp);
        }
        set->buckets[i] = NULL; // Reset the bucket pointer
    }
}

void free_hash_set(HashSet *set)
{
    for (int i = 0; i < HASH_SET_SIZE; ++i) {
        Node *current = set->buckets[i];
        while (current != NULL) {
            Node *temp = current;
            current = current->next;
            free(temp->key);
            free(temp);
        }
    }
}

