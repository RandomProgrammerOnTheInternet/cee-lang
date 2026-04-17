#include "util.h"

[[noreturn]] void error(u16 code, char *message) {
	fprintf(stderr, "skill issue #%hu: ", code);
	fprintf(stderr, "%s\n", message);
	exit(1);
}

char *substr(char *str, u64 start, u64 end) {
	return strndup(str + start, end - start);
}
