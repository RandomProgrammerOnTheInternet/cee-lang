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

#endif // DS_H

#ifdef DS_IMPL

typedef struct arena_t {
	void *buf;
	size_t max_size;
	size_t ptr;
} arena_t;

void arena_push(arena_t *arena, void *item, size_t size) {
	if(size + arena->ptr > arena->max_size) {
		printf("error too much mem used\n");
		exit(1);
		//arena->buf = realloc(arena->buf, max_size * 2);
	}
	arena->buf[ptr] = item;
	arena->ptr += size;
}

#endif
