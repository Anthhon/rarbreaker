#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "hash_set.h"
#include "clock.h"

volatile bool password_found = false;

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
