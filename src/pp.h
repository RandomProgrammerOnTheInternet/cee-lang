#ifndef PP_H

#include <stdio.h>
#include "util.h"

char *remove_line(char *str, u64 start, u64 *i);
char *remove_comments(char *str);
char *preprocess(char *str);

#endif // PP_H

