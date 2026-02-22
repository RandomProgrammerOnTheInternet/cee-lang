#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <string.h>
#include "ds.h"

FILE *generate_asm(LIST(node_base_t) node);
void generate_return(LIST(node_base_t) node, FILE **file, size_t *i);
void generate_var_declaration(LIST(node_base_t) node, FILE **file, size_t *i);
void generate_label(LIST(node_base_t) node, FILE **file, size_t *i);
void generate_goto(LIST(node_base_t) node, FILE **file, size_t *i);

#endif // CODEGEN_H

#ifdef CODEGEN_IMPL

FILE *generate_asm(LIST(node_base_t) node) {
	FILE* file = fopen("out.asm", "a");
	fprintf(file, ".intel_syntax noprefix\n"
				  ".global _start\n"
				  "_start:\n");
	for(size_t i = 0; i < node.length; i++) {
		switch(node.value[i].statement_node.type) {
		case node_return:
			generate_return(node, &file, &i);
			break;
		case node_var_declaration:
			generate_var_declaration(node, &file, &i);
			break;
		case node_label:
			generate_label(node, &file, &i);
			break;
		case node_goto:
			generate_goto(node, &file, &i);
			break;
		}
	}
	LIST_FREE(node);
	
	return file;
}

void generate_return(LIST(node_base_t) node, FILE **file, size_t *i) {
	printf("called generate_return()\n");
	fprintf(*file, "	mov rax, 60\n");
	if(node.value[*i].statement_node.return_node.expression_node.type == node_int_lit) {
		fprintf(*file, "	mov edi, %s\n", node.value[*i].statement_node.return_node.expression_node.int_lit_node.token.value);
	}
	else {
		fprintf(*file, "	mov edi, dword ptr [rsp-%zu]\n", node.value[*i].statement_node.return_node.expression_node.var_node.stack_offset);
	}

	fprintf(*file, "	syscall\n");
}

void generate_var_declaration(LIST(node_base_t) node, FILE **file, size_t *i) {
	if(node.value[*i].statement_node.var_declaration_node.expression_node.type == node_int_lit) {
		fprintf(*file, "	mov dword ptr [rsp-%zu], %s\n", node.value[*i].statement_node.var_declaration_node.stack_offset,
															node.value[*i].statement_node.var_declaration_node.expression_node.int_lit_node.token.value);
	}
	else if(node.value[*i].statement_node.var_declaration_node.expression_node.type == node_var) {
		fprintf(*file, "	mov eax, dword ptr [rsp-%zu]\n", node.value[*i].statement_node.var_declaration_node.expression_node.var_node.stack_offset);
		fprintf(*file, "	mov dword ptr [rsp-%zu], eax\n", node.value[*i].statement_node.var_declaration_node.stack_offset);
	}
	else {
		printf("\ncodegen error var declaration\n");
		exit(1);
	}
}

void generate_label(LIST(node_base_t) node, FILE **file, size_t *i) {
	fprintf(*file, ".label_%s:\n", node.value[*i].statement_node.label_node.token.value);
}

void generate_goto(LIST(node_base_t) node, FILE **file, size_t *i) {
	fprintf(*file, "	jmp .label_%s\n", node.value[*i].statement_node.goto_node.token.value);
}

#endif // CODEGEN_IMPL
