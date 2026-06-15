#include "codegen.h"

#define print(msg, ...) print(msg __VA_OPT_(,) __VA_ARGS__)

static inline char *var(size_t stack_offset) {
	char *str;
	sprintf(str, "dword ptr [rsp-%zu]", stack_offset);
	return str;
}

static inline char *prim_expr(node_prim_expr_t prim_expr_node) {
	if(prim_expr_node.type == node_int_lit) {
		return prim_expr_node.int_lit_node->token.value;
	}
	else if(prim_expr_node.type == node_var) {
		return var(prim_expr_node.var_node.stack_offset);
	}
	else {
		LOG(PRN_YLW, "ERROR");
		exit(1);
	}
}

static inline void mov(char *s) {
	print("\tmov eax, %s\n", s);
}

static inline void jmp(char *s) {
	print("\tjmp %s\n", s);
}

static inline void imul(char *s) {
	print("\timul eax, %s\n", s);
}

static inline void add(char *s) {
	print("\tadd eax, %s\n", s);
}

static void generate_return(LIST(node_base_t) node, FILE **file, size_t *i);
static void generate_var_decl(LIST(node_base_t) node, FILE **file, size_t *i);
static void generate_label(LIST(node_base_t) node, FILE **file, size_t *i);
static void generate_goto(LIST(node_base_t) node, FILE **file, size_t *i);
static void generate_assignment(LIST(node_base_t) node, FILE **file, size_t *i);
static void generate_mul_expr();
static void generate_add_expr(node_expr_t expr, FILE **file, size_t *i, bool is_start);

FILE *generate_asm_x86(LIST(node_base_t) node) {
	LOG(PRN_YLW, "called generate_asm(): x86 backend");
	FILE* file = fopen("out.asm", "w");
	LOG(PRN_YLW, "opened file");
	print(".intel_syntax noprefix\n"
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
	LOG(PRN_YLW, "start");

	generate_expr(node, file, i);	
	print("\tmov rax, 60 # generate_return\n");
	print("\tsyscall # generate_return\n");

	LOG(PRN_YLW, "end");
}

void generate_var_decl(LIST(node_base_t) node, FILE **file, size_t *i) {
	LOG(PRN_YLW, "start");

	if(node.value[*i].statement_node->var_decl_node->expr_node->type == node_int_lit) {
		LOG(PRN_YLW, "expr is int_lit");
		print("\tmov dword ptr [rsp-%zu], %s # generate_var_decl\n",
			node.value[*i].statement_node->var_decl_node->stack_offset,
			node.value[*i].statement_node->var_decl_node->expr_node->int_lit_node->token.value);
	}
	else if(node.value[*i].statement_node->var_decl_node->expr_node->type == node_var) {
		LOG(PRN_YLW, "expr is var");
		print("\tmov eax, dword ptr [rsp-%zu] # generate_var_decl\n",
			node.value[*i].statement_node->var_decl_node->expr_node->var_node.stack_offset);
		print("\tmov dword ptr [rsp-%zu], eax # generate_var_decl\n",
			node.value[*i].statement_node->var_decl_node->stack_offset);
	}
	else if(node.value[*i].statement_node->var_decl_node->expr_node->type == node_bin_expr) {
		LOG(PRN_YLW, "expr is bin_expr");
		generate_bin_expr(*node.value[*i].statement_node->var_decl_node->expr_node, file, i, true);
		print("\tmov dword ptr [rsp-%zu], eax # generate_var_decl\n",
			node.value[*i].statement_node->var_decl_node->stack_offset);
	}
	else {
		printf("\ncodegen error var decl\n");
		exit(1);
	}

	LOG(PRN_YLW, "end");
}

void generate_label(LIST(node_base_t) node, FILE **file, size_t *i) {
	LOG(PRN_YLW, "start");

	print(".label_%s: # generate_label\n",
		node.value[*i].statement_node->label_node->token.value);

	LOG(PRN_YLW, "end");
}

void generate_goto(LIST(node_base_t) node, FILE **file, size_t *i) {
	LOG(PRN_YLW, "start");

	print("\tjmp .label_%s # generate_goto\n",
		node.value[*i].statement_node->goto_node->token.value);

	LOG(PRN_YLW, "end");
}

void generate_assignment(LIST(node_base_t) node, FILE **file, size_t *i) {
	LOG(PRN_YLW, "start");

	if(node.value[*i].statement_node->assignment_node->rhs->type == node_var) {
		LOG(PRN_YLW, "rhs is var");
		print("\tmov eax, dword ptr [rsp-%zu] # generate_assignment\n",
			node.value[*i].statement_node->assignment_node->lhs.stack_offset);
		print("\tmov dword ptr [rsp-%zu], eax # generate_assignment\n",
			node.value[*i].statement_node->assignment_node->rhs->var_node.stack_offset);
	}
	else if(node.value[*i].statement_node->assignment_node->rhs->type == node_int_lit) {
		LOG(PRN_YLW, "rhs is int_lit");
		print("\tmov dword ptr [rsp-%zu], %s # generate_assignment\n",
			node.value[*i].statement_node->assignment_node->lhs.stack_offset,
			node.value[*i].statement_node->assignment_node->rhs->int_lit_node->token.value);
	}
	else if(node.value[*i].statement_node->assignment_node->rhs->type == node_bin_expr) {
		LOG(PRN_YLW, "rhs is bin_expr");
		generate_bin_expr(*node.value[*i].statement_node->assignment_node->rhs, file, i, true);
		print("\tmov dword ptr [rsp-%zu], eax # generate_assignment\n",
			node.value[*i].statement_node->assignment_node->lhs.stack_offset);
	}
	else {
		printf("\ncodegen error assignment\n");
		exit(1);
	}

	LOG(PRN_YLW, "end");
}

void generate_mul_expr(LIST(node_base_t) base_node, FILE **file, size_t *i) {
	LOG(PRN_YLW, "start");
	if(node.value[*i].
	do {
		LOG(PRN_YLW, "loop");
		
	} while();

	LOG(PRN_YLW, "end");
}

void generate_add_expr(node_expr_t expr, FILE **file, bool is_start) {
	LOG(PRN_YLW, "start");

	switch(expr.add_expr_node->op) {
	case op_add:
		LOG(PRN_YLW, "op is add");
		if(is_start) {
			if(expr.add_expr_node->lhs->prim_expr_node->type == node_int_lit) {
				LOG(PRN_YLW, "lhs is int lit");
				print("\tmov eax, %s # generate_add_expr"\n",
					expr.add_expr_node->lhs->prim_expr_node->int_lit_node->token.value);
			}
			else if(expr.add_expr_node->lhs->prim_expr_node->type == node_var) {
				LOG(PRN_YLW, "lhs is var");
				print("\tmov eax, dword ptr [rsp-%zu] # generate_add_expr\n",
					expr.add_expr_node->lhs->prim_expr_node->var_node.stack_offset);
			}
			else {
				LOG(PRN_YLW, "ERROR");
				exit(1);
			}
		}
		if(expr.add_expr_node->rhs->type

		break;
	case op_sub:

		break;
	default:
		LOG(PRN_YLW, "ERROR");
		break;
	}
	
	LOG(PRN_YLW, "end");
}
