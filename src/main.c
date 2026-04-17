#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
// #define PP_IMPL
// #include "pp.h"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "util.h"

char *read_whole_file(FILE *f);
void compile(char **argv);

int main(int argc, char **argv) {
	if(argc == 1) {
		error(error_no_file, "you need a file bro");
	}
	compile(argv);
	LOG(PRN_BLU, "ended compilation");

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
	LOG(PRN_BLU, "opened file");
	char *src_str = read_whole_file(src_file);
	LOG(PRN_BLU, "converted file to string:\n%s", src_str);

	LIST(token_t) tokens = tokenize(src_str);
	LOG(PRN_BLU, "converted string to %lu tokens", tokens.length);

	LIST(node_base_t) base_node = parse(tokens);
	LOG(PRN_BLU, "converted tokens to nodes");
	for(size_t i = 0; i < variable_lookup.length; i++) {
		LOG(PRN_BLU, "variable name: %s | stack offset: %zu", variable_lookup.value[i].token.value, variable_lookup.value[i].stack_offset);
	}
	FILE *asm_file = generate_asm(base_node);
	LOG(PRN_BLU, "generated asm");
	fclose(asm_file);
	// system("as -o out.o out.asm && ld out.o");
	LOG(PRN_BLU, "assembled");

	free(src_str);
	fclose(src_file);
	LIST_FREE(tokens);
	LOG(PRN_BLU, "freed mem");
}
