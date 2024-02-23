#ifndef PASSWORD_H
#define PASSWORD_H

#include <stdint.h>

void password_reallocate(char *password, uint32_t pass_len);
void password_generate_random(char *password, char *characters, uint32_t pass_len, unsigned int seed);
void password_crack_random(char *filepath, char *characters, uint32_t pass_len);

#endif // PASSWORD_H
