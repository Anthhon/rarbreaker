#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#define strcasecmp _stricmp // Windows doesn't have strcasecmp, but _stricmp serves the same purpose
#else
#include <unistd.h>
#endif

#define START_CLOCK() _start = clock()
#define STOP_CLOCK() _end = clock()
#define CALCULATE_TIME() _cpu_time_used = ((double) (_end - _start) / CLOCKS_PER_SEC)
#define RAND_IN_RANGE(x) (rand() % (x))
#define HASH_SET_SIZE 100

typedef struct Node {
    char *key;
    struct Node *next;
} Node;

typedef struct {
    Node *buckets[HASH_SET_SIZE];
} HashSet;

volatile bool password_found = false;

long long current_time_ns() {
    #ifdef _WIN32
    return GetTickCount();
#else
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec * 1000000000LL + now.tv_nsec;
#endif
}

unsigned int hash_function(const char *key) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash * 31) + *key++;
    }
    return hash % HASH_SET_SIZE;
}

Node* create_node(const char *key) {
    Node *node = malloc(sizeof(Node));
    if (node != NULL) {
        node->key = strdup(key);
        node->next = NULL;
    }
    return node;
}

void hash_set_initialize(HashSet *set) {
    for (int i = 0; i < HASH_SET_SIZE; ++i) {
        set->buckets[i] = NULL;
    }
}

void set_insert(HashSet *set, const char *key) {
    unsigned int index = hash_function(key);
    Node *node = create_node(key);
    if (node == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    node->next = set->buckets[index];
    set->buckets[index] = node;
}

bool set_contains(HashSet *set, const char *key) {
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

void clean_hash_set(HashSet *set) {
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

void free_hash_set(HashSet *set) {
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

void password_reallocate(char *password, uint32_t pass_len)
{
    password = realloc(password, sizeof(*password) * pass_len + 1);
    if (password == NULL) {
        printf("[ERROR] Could not allocate \'%i\' to store new passwords.", pass_len);
        exit(EXIT_FAILURE);
    }

    memset(password, 0, pass_len + 1);
}

void password_generate(char *password, char *characters, uint32_t pass_len, unsigned int seed)
{
    srand(seed);
    uint32_t characters_l = strlen(characters);

    for (uint32_t i = 0; i < pass_len; ++i) {
        password[i] = characters[RAND_IN_RANGE(characters_l)];
    }
}

void password_crack_random(char *filepath, char *characters, uint32_t pass_len)
{
    clock_t _start = 0, _end = 0;
    double _cpu_time_used = 0;
    char *password = calloc(pass_len + 1, sizeof(*password));
    uint64_t tries_count = 0;
    uint64_t tries_max = pow(strlen(characters), pass_len);
    uint64_t tries_total = 0;
    HashSet set;
    hash_set_initialize(&set);

    srand(time(NULL));

    printf("Starting RANDOM bruteforce...\n");
    printf("File: %s\n", filepath);
    printf("Characters range: \"%s\"\n", characters);

    START_CLOCK();

    while (!password_found) {
        if (tries_count >= tries_max) {
            STOP_CLOCK();
            CALCULATE_TIME();
            printf("\n[INFO] Increasing password length to %i\n", pass_len);
#ifdef _WIN32
            printf("└ Calculated all %lli passwords combinations\n  within %i characters in %.3fs\n", tries_max, pass_len, _cpu_time_used);
#else
            printf("└ Calculated all %li passwords combinations\n  within %i characters in %.3fs\n", tries_max, pass_len, _cpu_time_used);
#endif
            ++pass_len;
            tries_total += tries_max;
            tries_max = pow(strlen(characters), pass_len);
            tries_count = 0;

            password_reallocate(password, pass_len);
            clean_hash_set(&set);
        }

        
        long long time_ns = current_time_ns();
        volatile unsigned int seed = (unsigned int)time_ns + (unsigned int)tries_count;
        password_generate(password, characters, pass_len, seed);

        if (set_contains(&set, password)) {
            //printf("[!] Already used %s\n", password);
            continue;
        }

        //printf("\r%s\n", password);
        fflush(stdout);
        set_insert(&set, password);

        // If you try a password greater than
        // 100 characters ur already crazy
        char command[1000];

        sprintf(command, "unzip -P %s -o %s > /dev/null 2>&1", password, filepath);

        int unzip_result = system(command);
        if (unzip_result == 0) {
            password_found = true;
        }

        ++tries_count;
    }
    tries_total += tries_count;

    STOP_CLOCK();
    CALCULATE_TIME();

    printf("\n[Password] %s\n", password);
    printf("└ Password found!\n");
    printf("└ Time taken: %.3fs\n", _cpu_time_used);

#ifdef _WIN32
    printf("└ Found after %lli tries.\n", tries_total);
#else
    printf("└ Found after %li tries.\n", tries_total);
#endif

    free(password);
    password = NULL;
    free_hash_set(&set);
}

void print_help(char *program_name) {
    printf("Usage: %s -c <characters> -l <pass_len> -f <filepath>\n", program_name);
    printf("Options:\n");
    printf("  -c <characters>   Characters range for password generation\n");
    printf("  -l <pass_len>     Length of the password to generate\n");
    printf("  -f <filepath>     Path to the file to crack\n");
    printf("  -h               printf(\"\nExamples:\n\")");
    printf("  %s -c abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 -l 8 -f encrypted.zip\n", program_name);
    printf("      This command attempts to crack the password for the file 'encrypted.zip' by generating random passwords\n");
    printf("      using the characters provided and with a length of 8.\n");
}

int main(int argc, char *argv[])
{
    char *characters = NULL;
    char *filepath = NULL;
    uint32_t pass_len = 1;
    bool missing_args = false;

    if (argc < 4 || (argc == 2 && strcmp(argv[1], "-h") == 0)) {
        print_help(argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-c") == 0) {
            characters = argv[++i];
        } else if (strcmp(argv[i], "-l") == 0) {
            pass_len = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-f") == 0) {
            filepath = argv[++i];
        }
    }

    if (characters == NULL || pass_len == 0 || filepath == NULL) {
        fprintf(stderr, "Missing argument.\n");
        missing_args = true;
    }

    if (missing_args) {
        fprintf(stderr, "Usage: %s -c <characters> -l <pass_len> -f <filepath>\n", argv[0]);
        return 1;
    }

    #ifdef _WIN32
    FILE *file = fopen(filepath, "rb"); // Windows uses "rb" for binary mode
#else
    FILE *file = fopen(filepath, "r");
#endif
    if (file == NULL) {
        printf("File %s not found.\n", filepath);
        fprintf(stderr, "Usage: %s -c <characters> -l <pass_len> -f <filepath>\n", argv[0]);
        return 1;
    }
    fclose(file);

    password_crack_random(filepath, characters, pass_len);

    return 0;
}
