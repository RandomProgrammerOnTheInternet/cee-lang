#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <string.h>
#include "ds.h"

int asprintf(char **restrict strp, const char *restrict fmt, ...); // ...
char *generate_asm(LIST(node_base_t) node);

#endif // CODEGEN_H

#ifdef CODEGEN_IMPL

char *generate_asm(LIST(node_base_t) node) {
	char* str;
	asprintf(&str, ".intel_syntax noprefix\n"
				   ".global _start\n"
				   "_start:\n"
				   "	mov rax, 60\n"
				   "	mov rdi, %s\n"
				   "	syscall\n",
				   node.value[0].statement_node.return_node.expression_node.int_lit_node.token.value);
	LIST_FREE(node);
	
	return str;
}

#endif // CODEGEN_IMPL
