#include "codegen.h"

#define print(msg, ...) fprintf(asm_file, msg __VA_OPT__(,) __VA_ARGS__)

FILE *asm_file = NULL;
static const char *expr_reg[] = {"eax", "ecx", "edx", "ebx", "rsi", "rdi"};

static void generate_fn_decl(node_fn_decl_t node);
static void generate_statement(node_statement_t node);
static void generate_compound_statement(node_compound_statement_t node);
static void generate_if(node_if_t node);
static void generate_while(node_while_t node);
static void generate_for(node_for_t node);
static void generate_return(node_return_t node);
static void generate_var_decl(node_var_decl_t node);
static void generate_label(node_label_t node);
static void generate_goto(node_goto_t node);
static void generate_prim_expr(char **dest, node_prim_expr_t expr);
static void generate_post_expr(char **dest, node_post_expr_t expr);
static void generate_mul_expr(char **dest, node_mul_expr_t expr);
static void generate_add_expr(char **dest, node_add_expr_t expr);
static void generate_relat_expr(char **dest, node_relat_expr_t expr);
static void generate_equal_expr(char **dest, node_equal_expr_t expr);
static void generate_assign_expr(char **dest, node_assign_expr_t node);
static void generate_expr(node_expr_t expr);

static inline char **next_expr_reg(char **reg) {
	return reg + 1;
}

/* instructions */

static inline char *var(size_t stack_offset) {
	char *str = malloc(24); // size of string (17) + extra for number digits (7 digits)
	sprintf(str, "dword ptr [rbp-%zu]", stack_offset);
	return str;
}

static inline char *prim_expr(node_prim_expr_t prim_expr_node) {
	if(prim_expr_node.type == node_int_lit) {
		return prim_expr_node.int_lit_node->token.value;
	}
	else if(prim_expr_node.type == node_var) {
		LOG(PRN_YLW, "%zu", prim_expr_node.obj.stack_offset);
		return var(prim_expr_node.obj.stack_offset);
	}
	else {
		LOG(PRN_YLW, "ERROR");
		exit(1);
	}
}

static inline void call(char *fn) {
	print("\tcall %s\n", fn);
}

static inline void and(char *dest, char *src) {
	print("\tand %s, %s\n", dest, src);
}

static inline void mov(char *dest, char *src) {
	print("\tmov %s, %s\n", dest, src);
}

static inline void movzx(char *dest, char *src) {
	print("\tmovzx %s, %s\n", dest, src);
}

static inline void cmp(char *lhs, char *rhs) {
	print("\tcmp %s, %s\n", lhs, rhs);
}

static inline void test(char *lhs, char *rhs) {
	print("\ttest %s, %s\n", lhs, rhs);
}

static inline void sete(char *s) {
	print("\tsete %s\n", s);
}

static inline void setne(char *s) {
	print("\tsetne %s\n", s);
}

static inline void setg(char *s) {
	print("\tsetg %s\n", s);
}

static inline void setge(char *s) {
	print("\tsetge %s\n", s);
}

static inline void setl(char *s) {
	print("\tsetl %s\n", s);
}

static inline void setle(char *s) {
	print("\tsetle %s\n", s);
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

FILE *generate_asm_x86(LIST(node_base_t) node) {
	LOG(PRN_YLW, "called generate_asm(): x86 backend");
	asm_file = fopen("out.asm", "w");
	LOG(PRN_YLW, "opened file");
	print(".intel_syntax noprefix\n");
	for(size_t i = 0; i < node.length; i++) {
		LOG(PRN_YLW, "loop");
		generate_fn_decl(*node.value[i].fn_decl_node);
	}
//	LIST_FREE(node);
	
	return asm_file;
}

void generate_fn_decl(node_fn_decl_t node) {
	LOG(PRN_YLW, "start");
	print(".global %s\n", node.token.value);
	print("%s:\n", node.token.value);
	print("\tpush rbp\n");
	print("\tmov rbp, rsp\n");
	generate_compound_statement(*node.body);

	LOG(PRN_YLW, "end");
}

void generate_statement(node_statement_t node) {
	LOG(PRN_YLW, "start");
	switch(node.type) {
	case node_return:
		LOG(PRN_YLW, "detected node_return");
		generate_return(*node.return_node);
		break;
	case node_var_decl:
		LOG(PRN_YLW, "detected node_var_decl");
		generate_var_decl(*node.var_decl_node);
		break;
	case node_label:
		LOG(PRN_YLW, "detected node_label");
		generate_label(*node.label_node);
		break;
	case node_goto:
		LOG(PRN_YLW, "detected node_goto");
		generate_goto(*node.goto_node);
		break;
	case node_compound_statement:
		LOG(PRN_YLW, "detected node_compound_statement");
		generate_compound_statement(*node.compound_statement_node);
		break;
	case node_if:
		LOG(PRN_YLW, "detected node_if");
		generate_if(*node.if_node);
		break;
	case node_while:
		LOG(PRN_YLW, "detected node_while");
		generate_while(*node.while_node);
		break;
	case node_for:
		LOG(PRN_YLW, "detected node_for");
		generate_for(*node.for_node);
		break;
	case node_expr:
		LOG(PRN_YLW, "detected node_expr");
		generate_expr(*node.expr_node);
		break;
	default:
		LOG(PRN_YLW, "default");
		break;
	}
}

void generate_compound_statement(node_compound_statement_t node) {
	LOG(PRN_YLW, "start");
	LOG(PRN_YLW, "%zu", node.statement_nodes.length);

	for(size_t i = 0; i < node.statement_nodes.length; i++) {
		LOG(PRN_YLW, "loop");
		generate_statement(*node.statement_nodes.value[i]);
	}
	LOG(PRN_YLW, "end");
}


void generate_if(node_if_t node) {
	LOG(PRN_YLW, "start");
	static size_t num = 0;
	LOG(PRN_YLW, "num = %zu", num);

	generate_expr(*node.expr_node);
	test("eax", "eax"); // thank you therealblue24 for this tip
	print("\tje .Lif%zu\n", num);
	generate_statement(*node.if_branch);

	if(node.type == node_if_else) {
		print("\tjmp .Lif%zu\n", num + 1);
	}

	print(".Lif%zu:\n", num);
	num++;
	if(node.type == node_if_else) {
		generate_statement(*node.else_branch);
		print(".Lif%zu:\n", num);
		num++;
	}

	LOG(PRN_YLW, "end");
}

void generate_while(node_while_t node) {
	LOG(PRN_YLW, "start");
	static size_t num = 0;
	LOG(PRN_YLW, "num = %zu", num);
	if(node.type == node_while) {
		print(".Lwhile%zu:\n", num);
		generate_expr(*node.expr_node);
		test("eax", "eax");
		print("\tje .Lwhile%zu\n", num + 1);
		generate_statement(*node.body);
		print("\tjmp .Lwhile%zu\n", num);
		num++;
		print(".Lwhile%zu:\n", num);
		num++;
	}
	else {
		print(".Lwhile%zu:\n", num);
		generate_statement(*node.body);
		generate_expr(*node.expr_node);
		test("eax", "eax");
		print("\tjne .Lwhile%zu\n", num);
		num++;
	}

	LOG(PRN_YLW, "end");
}

void generate_for(node_for_t node) {
	LOG(PRN_YLW, "start");
	static size_t num = 0;
	LOG(PRN_YLW, "num = %zu", num);
	if(node.type == node_decl_for) {
		generate_var_decl(*node.var_decl_node);
	}
	else {
		generate_expr(*node.expr1);
	}
	print(".Lfor%zu:\n", num);
	generate_expr(*node.expr2);
	test("eax", "eax");
	print("\tje .Lfor%zu\n", num + 1);
	generate_statement(*node.body);
	generate_expr(*node.expr3);
	print("\tjmp .Lfor%zu\n", num);
	num++;
	print(".Lfor%zu:\n", num);
	num++;
	
	LOG(PRN_YLW, "end");
}

void generate_return(node_return_t node) {
	LOG(PRN_YLW, "start");

	generate_expr(*node.expr_node);	
	mov("edi", "eax");
	print("\tpop rbp\n");
	print("\tret\n");

	LOG(PRN_YLW, "end");
}

void generate_var_decl(node_var_decl_t node) {
	LOG(PRN_YLW, "start");

	generate_expr(*node.expr_node);
	mov(var(node.stack_offset), "eax");

	LOG(PRN_YLW, "end");
}

void generate_label(node_label_t node) {
	LOG(PRN_YLW, "start");

	print(".label_%s: # generate_label\n",
		node.token.value);

	LOG(PRN_YLW, "end");
}

void generate_goto(node_goto_t node) {
	LOG(PRN_YLW, "start");

	print("\tjmp .label_%s # generate_goto\n",
		node.token.value);

	LOG(PRN_YLW, "end");
}

void generate_prim_expr(char **dest, node_prim_expr_t expr) {
	LOG(PRN_YLW, "start");
	switch(expr.type) {
	case node_int_lit:
		LOG(PRN_YLW, "node_int_lit");
		mov("eax", expr.int_lit_node->token.value);
		break;
	case node_var:
		LOG(PRN_YLW, "node_var");
		LOG(PRN_YLW, "%zu", expr.obj.stack_offset);
		mov("eax", var(expr.obj.stack_offset));
		break;
	case node_fn_call:
		LOG(PRN_YLW, "node_fn_call");
		print("\tlea rax, [rip + %s]\n", expr.obj.token.value);
		break;
	}
	LOG(PRN_YLW, "end");
}

void generate_post_expr(char **dest, node_post_expr_t expr) {
	LOG(PRN_YLW, "start");

	if(expr.type == node_post_expr) {
		LOG(PRN_YLW, "expr.type == node_post_expr");
		generate_post_expr(dest, *expr.post_expr_node);
	}
	else {
		LOG(PRN_YLW, "expr.type == node_prim_expr");
		generate_prim_expr(dest, *expr.prim_expr_node);
		goto end;
	}
	switch(expr.op) {
	case op_fun:
		LOG(PRN_YLW, "op_fun");
		call("rax");
		break;
	case op_inc:
		LOG(PRN_YLW, "op_inc");
		add("eax", "1");
		mov(var(expr.post_expr_node->prim_expr_node->obj.stack_offset), "eax");
		break;
	case op_dec:
		LOG(PRN_YLW, "op_dec");
		sub("eax", "1");
		mov(var(expr.post_expr_node->prim_expr_node->obj.stack_offset), "eax");
		break;
	default:
		LOG(PRN_YLW, "ERROR: %d", expr.op);
		exit(1);
	}

end:
	LOG(PRN_YLW, "end");
}

void generate_mul_expr(char **dest, node_mul_expr_t expr) {
	LOG(PRN_YLW, "start");

	if(expr.type == node_mul_expr) {
		LOG(PRN_YLW, "expr.type == node_mul_expr");
		generate_mul_expr(dest, *expr.lhs);
	}
	else {
		LOG(PRN_YLW, "expr.type == node_post_expr");
		generate_post_expr(dest, *expr.post_expr_node);
		goto end;
	}
	
	if(expr.rhs->type == node_post_expr) {
		LOG(PRN_YLW, "expr.rhs->type == node_post_expr");
		generate_post_expr(next_expr_reg(dest), *expr.rhs);
		switch(expr.op) {
		case op_mul:
			LOG(PRN_YLW, "op_mul with rhs");
			imul(*dest, *next_expr_reg(dest));
			break;
		case op_div:
			LOG(PRN_YLW, "op_div with rhs");
			printf("sorry division and modulus doesnt work yet, x86 is weird\n");
			exit(1);
			idiv(*next_expr_reg(dest));
			break;
		case op_mod:
			LOG(PRN_YLW, "op_mod with rhs");
			printf("sorry division and modulus doesnt work yet, x86 is weird\n");
			exit(1);
			idiv(*next_expr_reg(dest));
			mov(*dest, "edx");
			break;
		}
	}
	switch(expr.op) {
	case op_mul:
		LOG(PRN_YLW, "op_mul");
		imul(*dest, prim_expr(*expr.rhs->prim_expr_node));
		break;
	case op_div:
		LOG(PRN_YLW, "op_div");
		printf("sorry division and modulus doesnt work yet, x86 is weird\n");
		exit(1);
		idiv(prim_expr(*expr.rhs->prim_expr_node));
		break;
	case op_mod:
		LOG(PRN_YLW, "op_mod");
		printf("sorry division and modulus doesnt work yet, x86 is weird\n");
		exit(1);
		idiv(prim_expr(*expr.rhs->prim_expr_node));
		mov(*dest, "edx");
		break;
	}

end:
	LOG(PRN_YLW, "end");
}

void generate_add_expr(char **dest, node_add_expr_t expr) {
	LOG(PRN_YLW, "start");

	if(expr.type == node_add_expr) {
		LOG(PRN_YLW, "expr.type == node_add_expr");
		generate_add_expr(dest, *expr.lhs);
	}
	else {
		LOG(PRN_YLW, "expr.type == node_mul_expr");
		generate_mul_expr(dest, *expr.mul_expr_node);
		goto end;
	}

	if(expr.rhs->type == node_mul_expr) {
		LOG(PRN_YLW, "expr.rhs->type == node_mul_expr");
		generate_mul_expr(next_expr_reg(dest), *expr.rhs);
		switch(expr.op) {
		case op_add:
			LOG(PRN_YLW, "op_add with rhs");
			add(*dest, *next_expr_reg(dest));
			goto end;
		case op_sub:
			LOG(PRN_YLW, "op_sub with rhs");
			sub(*dest, *next_expr_reg(dest));
			goto end;
		default:
			LOG(PRN_GRN, "ERROR????");
			exit(1);
		}
	}

	switch(expr.op) {
	case op_add:
		LOG(PRN_YLW, "op_add");
		add(*dest, prim_expr(*expr.rhs->post_expr_node->prim_expr_node));
		break;
	case op_sub:
		LOG(PRN_YLW, "op_sub");
		sub(*dest, prim_expr(*expr.rhs->post_expr_node->prim_expr_node));
		break;
	}
	
end:
	LOG(PRN_YLW, "end");
}

void generate_relat_expr(char **dest, node_relat_expr_t expr) {
	LOG(PRN_YLW, "start");

	if(expr.type == node_relat_expr) {
		LOG(PRN_YLW, "expr.type == node_relat_expr");
		generate_relat_expr(dest, *expr.lhs);
	}
	else {
		LOG(PRN_YLW, "expr.type == node_add_expr");
		generate_add_expr(dest, *expr.add_expr_node);
		goto end;
	}

	if(expr.rhs->type == node_add_expr) {
		LOG(PRN_YLW, "expr.rhs->type == node_add_expr");
		generate_add_expr(next_expr_reg(dest), *expr.rhs);
		switch(expr.op) {
		case op_g:
			LOG(PRN_YLW, "op_g with rhs");
			cmp(*dest, *next_expr_reg(dest));
			setg("al");
			movzx(*dest, "al");
			goto end;
		case op_ge:
			LOG(PRN_YLW, "op_ge with rhs");
			cmp(*dest, *next_expr_reg(dest));
			setge("al");
			movzx(*dest, "al");
			goto end;
		case op_l:
			LOG(PRN_YLW, "op_l with rhs");
			cmp(*dest, *next_expr_reg(dest));
			setl("al");
			movzx(*dest, "al");
			goto end;
		case op_le:
			LOG(PRN_YLW, "op_le with rhs");
			cmp(*dest, *next_expr_reg(dest));
			setle("al");
			movzx(*dest, "al");
			goto end;
		}
	}
	switch(expr.op) {
	case op_g:
		LOG(PRN_YLW, "op_g");
		cmp(*dest, prim_expr(*expr.rhs->mul_expr_node->post_expr_node->prim_expr_node));
		setg("al");
		movzx(*dest, "al");
		goto end;
	case op_ge:
		LOG(PRN_YLW, "op_ge");
		cmp(*dest, prim_expr(*expr.rhs->mul_expr_node->post_expr_node->prim_expr_node));
		setge("al");
		movzx(*dest, "al");
		goto end;
	case op_l:
		LOG(PRN_YLW, "op_l");
		cmp(*dest, prim_expr(*expr.rhs->mul_expr_node->post_expr_node->prim_expr_node));
		setl("al");
		movzx(*dest, "al");
		goto end;
	case op_le:
		LOG(PRN_YLW, "op_le");
		cmp(*dest, prim_expr(*expr.rhs->mul_expr_node->post_expr_node->prim_expr_node));
		setle("al");
		movzx(*dest, "al");
		goto end;
	}
	
end:
	LOG(PRN_YLW, "end");
}

void generate_equal_expr(char **dest, node_equal_expr_t expr) {
	LOG(PRN_YLW, "start");

	if(expr.type == node_equal_expr) {
		LOG(PRN_YLW, "expr.type == node_equal_expr");
		generate_equal_expr(dest, *expr.lhs);
	}
	else {
		LOG(PRN_YLW, "expr.type == node_relat_expr");
		generate_relat_expr(dest, *expr.relat_expr_node);
		goto end;
	}

	if(expr.rhs->type == node_relat_expr) {
		LOG(PRN_YLW, "expr.rhs->type == node_relat_expr");
		generate_relat_expr(next_expr_reg(dest), *expr.rhs);
		switch(expr.op) {
		case op_equ:
			LOG(PRN_YLW, "op_equ with rhs");
			cmp(*dest, *next_expr_reg(dest));
			sete("al");
			movzx(*dest, "al");
			goto end;
		case op_neq:
			LOG(PRN_YLW, "op_neq with rhs");
			cmp(*dest, *next_expr_reg(dest));
			setne("al");
			movzx(*dest, "al");
			goto end;
		}
	}

	switch(expr.op) {
	case op_equ:
		LOG(PRN_YLW, "op_equ");
		cmp(*dest, prim_expr(*expr.rhs->add_expr_node->mul_expr_node->post_expr_node->prim_expr_node));
		sete("al");
		movzx(*dest, "al");
		break;
	case op_neq:
		LOG(PRN_YLW, "op_neq");
		cmp(*dest, prim_expr(*expr.rhs->add_expr_node->mul_expr_node->post_expr_node->prim_expr_node));
		setne("al");
		movzx(*dest, "al");
		break;
	}

end:
	LOG(PRN_YLW, "end");
}

void generate_assign_expr(char **dest, node_assign_expr_t node) {
	LOG(PRN_YLW, "start");

	if(node.type == node_equal_expr) {
		generate_equal_expr(dest, *node.equal_expr_node);
	}
	else {
		generate_equal_expr(dest, *node.rhs);
		mov(var(node.lhs.stack_offset), "eax");
	}

	LOG(PRN_YLW, "end");
}


void generate_expr(node_expr_t expr) {
	LOG(PRN_YLW, "start");
	generate_assign_expr(&expr_reg[0], *expr.assign_expr_node);
	LOG(PRN_YLW, "end");
}
