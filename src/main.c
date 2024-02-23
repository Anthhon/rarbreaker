#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "password.h"

#ifdef _WIN32
#include <windows.h>
#define strcasecmp _stricmp // Windows doesn't have strcasecmp, but _stricmp serves the same purpose
#else
#include <unistd.h>
#endif

void print_help(char *program_name)
{
    printf("Usage: %s -c <characters> -l <pass_len> -f <filepath>\n", program_name);
    printf("Options:\n");
    printf("  -c <characters>   Characters range for password generation\n");
    printf("  -l <pass_len>     Length of the password to generate\n");
    printf("  -f <filepath>     Path to the file to crack\n");
    printf("  -r                Uses random generation to brute force password (can be more efficient some times).\n");
    printf("  -h                Print help page");
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
    bool random_mode = false;

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
        } else if (strcmp(argv[i], "-r") == 0) {
            random_mode = true;
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

    if (random_mode) password_crack_random(filepath, characters, pass_len);
    else password_crack_linear(filepath, characters, pass_len);

    return 0;
}
