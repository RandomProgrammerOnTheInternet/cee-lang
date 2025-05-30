#ifndef DS_H
#define DS_H

#include <stdlib.h>

// Dynamic Arrays
#define NEW_LIST(x) struct list_ ## x {x* value; size_t size;}

#define LIST(x) struct list_ ## x

#define INIT_LIST(x, y) \
x.size = y * sizeof(typeof(*x.value)); \
x.value = malloc(x.size);

#define LIST_RESIZE(x, y) \
x.size = y * sizeof(typeof(*x.value)); \
x.value = realloc(x.value, x.size)

#define LIST_ADD(x, y) LIST_RESIZE(x, (x.size / sizeof(typeof(*x.value)) + y))

#define LIST_APPEND(x, y) LIST_ADD(x, 1); \
x.value[x.size / sizeof(typeof(*x.value)) - 1] = y

#define LIST_FREE(x) if(x.value != NULL) free(x.value)

#endif // DS_H
