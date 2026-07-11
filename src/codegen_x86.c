#include "codegen.h"

#define print(msg, ...) fprintf(asm_file, msg __VA_OPT__(,) __VA_ARGS__)

FILE *asm_file = NULL;

static inline char *var(size_t stack_offset) {
	char *str = NULL;
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

static inline void mov(char *dest, char *source) {
	print("\tmov %s, %s\n", dest, source);
}

static inline void jmp(char *s) {
	print("\tjmp %s\n", s);
}

static inline void imul(char *lhs, char *rhs) {
	print("\timul %s, %s\n", lhs, rhs);
}

static inline void idiv(char *s) {
	print("\tidiv %s\n", s);
}

static inline void add(char *lhs, char *rhs) {
	print("\tadd %s, %s\n", lhs, rhs);
}

static inline void sub(char *lhs, char *rhs) {
	print("\tsub %s, %s\n", lhs, rhs);
}

static void generate_return(LIST(node_base_t) node, size_t *i);
static void generate_var_decl(LIST(node_base_t) node, size_t *i);
static void generate_label(LIST(node_base_t) node, size_t *i);
static void generate_goto(LIST(node_base_t) node, size_t *i);
static void generate_assignment(LIST(node_base_t) node, size_t *i);
static void generate_mul_expr(char *dest, node_mul_expr_t expr);
static void generate_add_expr(node_add_expr_t expr);
static void generate_expr(node_expr_t expr);

FILE *generate_asm_x86(LIST(node_base_t) node) {
	LOG(PRN_YLW, "called generate_asm(): x86 backend");
	asm_file = fopen("out.asm", "w");
	LOG(PRN_YLW, "opened file");
	print(".intel_syntax noprefix\n"
		  ".global _start\n"
		  "_start:\n");
	for(size_t i = 0; i < node.length; i++) {
		LOG(PRN_YLW, "loop");
		switch(node.value[i].statement_node->type) {
		case node_return:
			LOG(PRN_YLW, "detected node_return");
			generate_return(node, &i);
			break;
		case node_var_decl:
			LOG(PRN_YLW, "detected node_var_decl");
			generate_var_decl(node, &i);
			break;
		case node_label:
			LOG(PRN_YLW, "detected node_label");
			generate_label(node, &i);
			break;
		case node_goto:
			LOG(PRN_YLW, "detected node_goto");
			generate_goto(node, &i);
			break;
		case node_assignment:
			LOG(PRN_YLW, "detected node_assignment");
			generate_assignment(node, &i);
			break;
		default:
			LOG(PRN_YLW, "default");
			break;
		}
	}
//	LIST_FREE(node);
	
	return asm_file;
}

void generate_return(LIST(node_base_t) node, size_t *i) {
	LOG(PRN_YLW, "start");
	print("# begin %s\n", __func__);

	generate_expr(*node.value[*i].statement_node->return_node->expr_node);	
	mov("edi", "eax");
	mov("rax", "60");
	print("\tsyscall\n");

	print("# end %s\n", __func__);
	LOG(PRN_YLW, "end");
}

void generate_var_decl(LIST(node_base_t) node, size_t *i) {
	LOG(PRN_YLW, "start");
	print("# begin %s\n", __func__);

	generate_expr(*node.value[*i].statement_node->var_decl_node->expr_node);
	mov(var(node.value[*i].statement_node->var_decl_node->stack_offset), "eax");

/*
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
	*/

	print("# end %s\n", __func__);
	LOG(PRN_YLW, "end");
}

void generate_label(LIST(node_base_t) node, size_t *i) {
	LOG(PRN_YLW, "start");
	print("# begin %s\n", __func__);

	print(".label_%s: # generate_label\n",
		node.value[*i].statement_node->label_node->token.value);

	print("# end %s\n", __func__);
	LOG(PRN_YLW, "end");
}

void generate_goto(LIST(node_base_t) node, size_t *i) {
	LOG(PRN_YLW, "start");
	print("# begin %s\n", __func__);

	print("\tjmp .label_%s # generate_goto\n",
		node.value[*i].statement_node->goto_node->token.value);

	print("# end %s\n", __func__);
	LOG(PRN_YLW, "end");
}

void generate_assignment(LIST(node_base_t) node, size_t *i) {
	LOG(PRN_YLW, "start");
	print("# begin %s\n", __func__);

	generate_expr(*node.value[*i].statement_node->assignment_node->rhs);
	mov(var(node.value[*i].statement_node->assignment_node->lhs.stack_offset), "eax");

/*
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
	*/

	print("# end %s\n", __func__);
	LOG(PRN_YLW, "end");
}

void generate_mul_expr(char *dest, node_mul_expr_t expr) {
	LOG(PRN_YLW, "start");

	if(expr.type == node_mul_expr) {
		generate_mul_expr(dest, *expr.lhs);
	}
	else {
		mov(dest, prim_expr(*expr.prim_expr_node));
		LOG(PRN_YLW, "end");
		return;
	}

	switch(expr.op) {
	case op_mul:
		imul(dest, prim_expr(*expr.rhs));
		break;
	case op_div:
		idiv(prim_expr(*expr.rhs));
		break;
	case op_mod:
		idiv(prim_expr(*expr.rhs));
		mov(dest, "edx");
		break;
	}

	LOG(PRN_YLW, "end");
}

void generate_add_expr(node_add_expr_t expr) {
	LOG(PRN_YLW, "start");

	if(expr.type == node_add_expr) {
		generate_add_expr(*expr.lhs);
	}
	else {
		if(expr.mul_expr_node->type == node_prim_expr) {
			mov("eax", prim_expr(*expr.mul_expr_node->prim_expr_node));
		}
		else {
			generate_mul_expr("eax", *expr.mul_expr_node);
		}
		LOG(PRN_YLW, "end");
		return;
	}

	if(expr.rhs->type == node_mul_expr) {
		generate_mul_expr("ecx", *expr.rhs);
		add("eax", "ecx");
	}
	/*
	else {
		add("eax", prim_expr(*expr.rhs->prim_expr_node));
	}
	*/

	switch(expr.op) {
	case op_add:
		add("eax", prim_expr(*expr.rhs->prim_expr_node));
		break;
	case op_sub:
		sub("eax", prim_expr(*expr.rhs->prim_expr_node));
		break;
	}
	
	LOG(PRN_YLW, "end");
}

void generate_expr(node_expr_t expr) {
	generate_add_expr(*expr.add_expr_node);
}
