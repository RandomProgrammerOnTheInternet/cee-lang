#ifndef DS_H
#define DS_H

#include <stdlib.h>

// Dynamic Arrays
#define NEW_LIST(x) struct list_ ## x {x* value; size_t length;}

#define LIST(x) struct list_ ## x

#define INIT_LIST(x, y) \
x.length = y; \
x.value = malloc(x.length * sizeof(typeof(*x.value)))

#define LIST_RESIZE(x, y) \
x.length = y; \
x.value = realloc(x.value, x.length * sizeof(typeof(*x.value)))

#define LIST_ADD(x, y) LIST_RESIZE(x, x.length + y)

#define LIST_APPEND(x, y) LIST_ADD(x, 1); \
x.value[x.length - 1] = y

#define LIST_FREE(x) if(x.value != NULL) free(x.value)

// arenas
#define NEW_ARENA(x) struct arena_ ## x {x *value; size_t length; size_t cap;}

#define ARENA(x) struct arena_ ## x

#define INIT_ARENA(x, y) \
x.length = 0; \
x.cap = y; \
x.value = malloc(x.length * sizeof(typeof(*x.value)))

#define ARENA_APPEND(x, y) \
x.value[x.length] = y; \
x.length++


#endif // DS_H

#ifdef DS_IMPL

#endif
