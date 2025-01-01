#include "util.h"
#include <stdio.h>
#include <stdlib.h>

char *read_whole_file(FILE *f) {
	  fseek(f, 0, SEEK_END);
	  long fsize = ftell(f);
	  fseek(f, 0, SEEK_SET);
	  char *str = malloc(fsize + 1);
	  fread(str, fsize, 1, f);
	  str[fsize] = 0;

	  return str;
}

char *char_push_back(char *str, char c, size_t size) {
    str = (char *)realloc(str, size);
    str[size - 1] = c;
    return str;
}

void safe_free(void *p) {
    if(p!=NULL) {
        free(p);
    }
}
