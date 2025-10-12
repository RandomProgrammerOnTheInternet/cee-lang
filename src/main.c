#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
// #define PP_IMPL
// #include "pp.h"
#define LEXER_IMPL
#include "lexer.h"
#define PARSER_IMPL
#include "parser.h"
#define CODEGEN_IMPL
#include "codegen.h"
#define UTIL_IMPL
#include "util.h"

char *read_whole_file(FILE *f);
void compile(char **argv);

int main(int argc, char **argv) {
	if(argc == 1) {
		error(error_no_file, "you need a file bro");
	}
	compile(argv);

	return 0;
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

void compile(char **argv) {
	FILE *src_file = NULL;
	if(!(src_file = fopen(argv[1], "r"))) {
		error(error_invalid_file, "invalid file");
	}
	char *src_str = read_whole_file(src_file);
	printf("Preprocessed File:\n%s\n", src_str);
	LIST(token_t) tokens = tokenize(src_str);
	printf("tokens.length = %lu\n", tokens.length);
	node_base_t base_node = parse(tokens);
	printf("base_node.return_node.int_lit.token.value %s\n", base_node.return_node.int_lit_node.token.value);
	char *asm_code = generate_asm(base_node);
	FILE *asm_file = fopen("out.asm", "w");
	fprintf(asm_file, "%s", asm_code);
	fclose(asm_file);
	free(asm_code);
	system("as -o out.o out.asm && ld out.o");

	printf("tokens: ");
	for(int i = 0; i < tokens.length; i++) {
		printf("%s ", tokens.value[i].value);
		free(tokens.value[i].value);
	}
	free(src_str);
	fclose(src_file);
	LIST_FREE(tokens);
}
