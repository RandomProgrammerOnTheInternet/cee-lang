#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef enum error_code : u16 {
	no_file,
	invalid_file,
	invalid_op
} error_code;

void error(error_code code, char *message);
char *substr(char *str, u64 start, u64 end);

#endif // UTIL_H

#ifdef UTIL_IMPL

void error(u16 code, char *message) {
	fprintf(stderr, "skill issue #%hu: ", code);
	fprintf(stderr, "%s\n", message);
	exit(1);
}

char *substr(char *str, u64 start, u64 end) {
	return strndup(str + start, end - start);
}

#endif // UTIL_IMPL
