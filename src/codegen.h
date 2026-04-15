#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <string.h>
#include "ds.h"

FILE *generate_asm(LIST(node_base_t) node);
void generate_return(LIST(node_base_t) node, FILE **file, size_t *i);
void generate_var_decl(LIST(node_base_t) node, FILE **file, size_t *i);
void generate_label(LIST(node_base_t) node, FILE **file, size_t *i);
void generate_goto(LIST(node_base_t) node, FILE **file, size_t *i);
void generate_assignment(LIST(node_base_t) node, FILE **file, size_t *i);
void generate_bin_expr(node_expr_t expr, FILE **file, size_t *i);
#endif // CODEGEN_H

#ifdef CODEGEN_IMPL

FILE *generate_asm(LIST(node_base_t) node) {
	LOG(PRN_YLW, "called generate_asm()");
	FILE* file = fopen("out.asm", "a");
	LOG(PRN_YLW, "opened file");
	fprintf(file, ".intel_syntax noprefix\n"
				  ".global _start\n"
				  "_start:\n");
	for(size_t i = 0; i < node.length; i++) {
		LOG(PRN_YLW, "loop");
		switch(node.value[i].statement_node->type) {
		case node_return:
			LOG(PRN_YLW, "detected node_return");
			generate_return(node, &file, &i);
			break;
		case node_var_decl:
			LOG(PRN_YLW, "detected node_var_decl");
			generate_var_decl(node, &file, &i);
			break;
		case node_label:
			LOG(PRN_YLW, "detected node_label");
			generate_label(node, &file, &i);
			break;
		case node_goto:
			LOG(PRN_YLW, "detected node_goto");
			generate_goto(node, &file, &i);
			break;
		case node_assignment:
			LOG(PRN_YLW, "detected node_assignment");
			generate_assignment(node, &file, &i);
			break;
		default:
			LOG(PRN_YLW, "default");
			break;
		}
	}
//	LIST_FREE(node);
	
	return file;
}

void generate_return(LIST(node_base_t) node, FILE **file, size_t *i) {
	LOG(PRN_YLW, "called generate_return()");
	if(node.value[*i].statement_node->return_node->expr_node->type == node_int_lit) {
		LOG(PRN_YLW, "expr is int_lit");
		fprintf(*file, "	mov edi, %s # generate_return\n", node.value[*i].statement_node->return_node->expr_node->int_lit_node->token.value);
	}
	else if(node.value[*i].statement_node->return_node->expr_node->type == node_bin_expr) {
		LOG(PRN_YLW, "expr is bin_expr");
		generate_bin_expr(*node.value[*i].statement_node->return_node->expr_node, file, i);
		fprintf(*file, "mov edi, eax # generate_return\n");
	}
	else {
		LOG(PRN_YLW, "expr is var");
		fprintf(*file, "	mov edi, dword ptr [rsp-%zu] # generate_return\n", node.value[*i].statement_node->return_node->expr_node->var_node.stack_offset);
	}

	fprintf(*file, "	mov rax, 60 # generate_return\n");
	fprintf(*file, "	syscall # generate_return\n");
	LOG(PRN_YLW, "syscalled");
}

void generate_var_decl(LIST(node_base_t) node, FILE **file, size_t *i) {
	LOG(PRN_YLW, "called generate_var_decl()");
	if(node.value[*i].statement_node->var_decl_node->expr_node->type == node_int_lit) {
		LOG(PRN_YLW, "expr is int_lit");
		fprintf(*file, "	mov dword ptr [rsp-%zu], %s # generate_var_decl\n", node.value[*i].statement_node->var_decl_node->stack_offset,
															node.value[*i].statement_node->var_decl_node->expr_node->int_lit_node->token.value);
	}
	else if(node.value[*i].statement_node->var_decl_node->expr_node->type == node_var) {
		LOG(PRN_YLW, "expr is var");
		fprintf(*file, "	mov eax, dword ptr [rsp-%zu] # generate_var_decl\n", node.value[*i].statement_node->var_decl_node->expr_node->var_node.stack_offset);
		fprintf(*file, "	mov dword ptr [rsp-%zu], eax # generate_var_decl\n", node.value[*i].statement_node->var_decl_node->stack_offset);
	}
	else if(node.value[*i].statement_node->var_decl_node->expr_node->type == node_bin_expr) {
		LOG(PRN_YLW, "expr is bin_expr");
		generate_bin_expr(*node.value[*i].statement_node->var_decl_node->expr_node, file, i);
		fprintf(*file, "	mov dword ptr [rsp-%zu], eax # generate_var_decl\n", node.value[*i].statement_node->var_decl_node->stack_offset);
	}
	else {
		printf("\ncodegen error var decl\n");
		exit(1);
	}
}

void generate_label(LIST(node_base_t) node, FILE **file, size_t *i) {
	LOG(PRN_YLW, "called generate_label()");
	fprintf(*file, ".label_%s: # generate_label\n", node.value[*i].statement_node->label_node->token.value);
}

void generate_goto(LIST(node_base_t) node, FILE **file, size_t *i) {
	LOG(PRN_YLW, "called generate_goto()");
	fprintf(*file, "	jmp .label_%s # generate_goto\n", node.value[*i].statement_node->goto_node->token.value);
}

void generate_assignment(LIST(node_base_t) node, FILE **file, size_t *i) {
	LOG(PRN_YLW, "called generate_assignment()");
	if(node.value[*i].statement_node->assignment_node->rhs->type == node_var) {
		LOG(PRN_YLW, "rhs is var");
		fprintf(*file, "	mov eax, dword ptr [rsp-%zu] # generate_assignment\n", node.value[*i].statement_node->assignment_node->lhs.stack_offset);
		fprintf(*file, "	mov dword ptr [rsp-%zu], eax # generate_assignment\n", node.value[*i].statement_node->assignment_node->rhs->var_node.stack_offset);
	}
	else if(node.value[*i].statement_node->assignment_node->rhs->type == node_int_lit) {
		LOG(PRN_YLW, "rhs is int_lit");
		fprintf(*file, "	mov dword ptr [rsp-%zu], %s # generate_assignment\n", node.value[*i].statement_node->assignment_node->lhs.stack_offset,
			node.value[*i].statement_node->assignment_node->rhs->int_lit_node->token.value);
	}
	else if(node.value[*i].statement_node->assignment_node->rhs->type == node_bin_expr) {
		LOG(PRN_YLW, "rhs is bin_expr");
		generate_bin_expr(*node.value[*i].statement_node->assignment_node->rhs, file, i);
		fprintf(*file, "	mov dword ptr [rsp-%zu], eax # generate_assignment\n", node.value[*i].statement_node->assignment_node->lhs.stack_offset);
	}
	else {
		printf("\ncodegen error assignment\n");
		exit(1);
	}
}

void generate_bin_expr(node_expr_t expr, FILE **file, size_t *i) {
	LOG(PRN_YLW, "called generate_bin_expr()");
	LOG(PRN_YLW, "%d", bin_expr_add);
	switch(expr.bin_expr_node->op) {
	case bin_expr_add:
		LOG(PRN_YLW, "op is add");
		if(expr.bin_expr_node->lhs->type == node_int_lit) {
			LOG(PRN_YLW, "lhs is int_lit");
			fprintf(*file, "	mov eax, %s # generate_bin_expr\n", expr.bin_expr_node->lhs->int_lit_node->token.value);
		}
		else if(expr.bin_expr_node->lhs->type == node_var) {
			LOG(PRN_YLW, "lhs is var");
			fprintf(*file, "	mov eax, dword ptr [rsp-%zu] # generate_bin_expr\n", expr.bin_expr_node->lhs->var_node.stack_offset);
		}
		else {
			LOG(PRN_YLW, "ERROR");
			exit(1);
		}
		if(expr.bin_expr_node->rhs->type == node_int_lit) {
			LOG(PRN_YLW, "rhs is int_lit");
			fprintf(*file, "	mov ecx, %s # generate_bin_expr\n", expr.bin_expr_node->rhs->int_lit_node->token.value);
		}
		else if(expr.bin_expr_node->rhs->type == node_var) {
			LOG(PRN_YLW, "rhs is var");
			fprintf(*file, "	mov ecx, dword ptr [rsp-%zu] # generate_bin_expr\n", expr.bin_expr_node->rhs->var_node.stack_offset);
		}
		else {
			LOG(PRN_YLW, "ERROR");
			exit(1);
		}
		fprintf(*file, "	add eax, ecx # generate_bin_expr\n");
		break;
	case bin_expr_sub:
		LOG(PRN_YLW, "op is sub");
		if(expr.bin_expr_node->lhs->type == node_int_lit) {
			LOG(PRN_YLW, "lhs is int_lit");
			fprintf(*file, "	mov eax, %s # generate_bin_expr\n", expr.bin_expr_node->lhs->int_lit_node->token.value);
		}
		else if(expr.bin_expr_node->lhs->type == node_var) {
			LOG(PRN_YLW, "lhs is var");
			fprintf(*file, "	mov eax, dword ptr [rsp-%zu] # generate_bin_expr\n", expr.bin_expr_node->lhs->var_node.stack_offset);
		}
		else {
			LOG(PRN_YLW, "ERROR");
			exit(1);
		}
		if(expr.bin_expr_node->rhs->type == node_int_lit) {
			LOG(PRN_YLW, "rhs is int_lit");
			fprintf(*file, "	mov ecx, %s # generate_bin_expr\n", expr.bin_expr_node->rhs->int_lit_node->token.value);
		}
		else if(expr.bin_expr_node->rhs->type == node_var) {
			LOG(PRN_YLW, "rhs is var");
			fprintf(*file, "	mov ecx, dword ptr [rsp-%zu] # generate_bin_expr\n", expr.bin_expr_node->rhs->var_node.stack_offset);
		}
		else {
			LOG(PRN_YLW, "ERROR");
			exit(1);
		}
		fprintf(*file, "	sub eax, ecx # generate_bin_expr\n");
		break;
	default:
		printf("\ngenerate_bin_expr\n");
		exit(1);
	}
}

#endif // CODEGEN_IMPL
