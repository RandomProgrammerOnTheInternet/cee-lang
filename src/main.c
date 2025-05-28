#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define PP_IMPL
#include "pp.h"
#define LEXER_IMPL
#include "lexer.h"

typedef enum error_code : u16 {
	no_file,
	invalid_file
} error_code;

void error(error_code code, char *message);
char *read_whole_file(FILE *f);
void compile(int argc, char **argv);

int main(int argc, char **argv) {
	compile(argc, argv);

	return 0;
}

void error(u16 code, char *message) {
	fprintf(stderr, "skill issue #%hu: ", code);
	fprintf(stderr, "%s\n", message);
	exit(1);
}

char *read_whole_file(FILE *f) {
	  fseek(f, 0, SEEK_END);
	  long fsize = ftell(f);
	  fseek(f, 0, SEEK_SET);
	  char *str = malloc(fsize + 1);
	  fread(str, fsize, 1, f);
	  str[fsize] = 0;

	  return str;
}

void compile(int argc, char **argv) {
	if(argc == 1) {
		error(no_file, "you need a file bro");
	}
	FILE *src_file = NULL;
	if(!(src_file = fopen(argv[1], "r"))) {
		error(invalid_file, "invalid file");
	}
	char *src_str = preprocess(read_whole_file(src_file));
	printf("%s\n", src_str);
	LIST(token_t) tokens = tokenize(src_str);
	printf("%lu\n", tokens.size);
	for(int i = 0; i < tokens.size / sizeof(tokens); i++) {
		printf("tokens.value[%d].value: %s\n", i, tokens.value[i].value);
		printf("tokens.value[%d].type:  %d\n", i, tokens.value[i].type);
		free(tokens.value[i].value);
	}
	free(src_str);
	fclose(src_file);
	LIST_FREE(tokens);
}
