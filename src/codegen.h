#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <string.h>
#include "ds.h"

int asprintf(char **restrict strp, const char *restrict fmt, ...); // ...
char *generate_asm(node_base_t node);

#endif // CODEGEN_H

#ifdef CODEGEN_IMPL

char *generate_asm(node_base_t node) {
	char* str;
	asprintf(&str, ".global _start\n"
				  "_start:\n"
				  "mov $60, %%rax\n"
				  "mov $%s, %%rdi\n"
				  "syscall\n",
				  node.return_node.int_lit_node.token.value);

	
	return str;
}

#endif // CODEGEN_IMPL
