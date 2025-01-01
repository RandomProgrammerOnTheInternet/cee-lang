#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>

char *read_whole_file(FILE*);
char *char_push_back(char*, char, size_t);
void safe_free(void*);

#endif // CEE_UTIL_H
