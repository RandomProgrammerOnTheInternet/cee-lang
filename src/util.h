#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef DEBUG
#define PRN_BLK "\e[0;30m"
#define PRN_RED "\e[0;31m"
#define PRN_GRN "\e[0;32m"
#define PRN_YLW "\e[0;33m"
#define PRN_BLU "\e[0;34m"
#define PRN_PUR "\e[0;35m"
#define PRN_CYN "\e[0;36m"
#define PRN_WHT "\e[0;37m"
#define LOG(color, msg, ...) \
printf("%s%d %s: %s", color, __LINE__, __func__, PRN_WHT); \
printf(msg __VA_OPT__(,) __VA_ARGS__); \
printf("\n")
#endif // DEBUG

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
	error_no_file,
	error_invalid_file,
	error_invalid_op,
	error_invalid_expression,
	error_missing_expression,
	error_missing_semicolon
} error_code;

[[noreturn]] void error(error_code code, char *message);
char *substr(char *str, u64 start, u64 end);

#endif // UTIL_H

#ifdef UTIL_IMPL

[[noreturn]] void error(u16 code, char *message) {
	fprintf(stderr, "skill issue #%hu: ", code);
	fprintf(stderr, "%s\n", message);
	exit(1);
}

char *substr(char *str, u64 start, u64 end) {
	return strndup(str + start, end - start);
}

#endif // UTIL_IMPL
