#include "codegen.h"
#include "parser.h"

static void generate_return(LIST(node_base_t) node, FILE **file, size_t *i);
static void generate_var_decl(LIST(node_base_t) node, FILE **file, size_t *i);
static void generate_label(LIST(node_base_t) node, FILE **file, size_t *i);
static void generate_goto(LIST(node_base_t) node, FILE **file, size_t *i);
static void generate_assignment(LIST(node_base_t) node, FILE **file, size_t *i);
static void generate_bin_expr(node_expr_t expr, FILE **file, size_t *i, bool is_start);

/* basic convention right now:
 * x8/w8 - accumulator
 * x9/w9 - temporary
 * x10/w10 - "accumulator" #2
 */


static size_t align_upto(size_t num, size_t align) {
	/* https://stackoverflow.com/questions/12721773/how-to-align-a-value-to-a-given-alignment */
	size_t r = num % align;
	return r ? num + (align - r) : num;
}

static void function_prelude(FILE *file, size_t stack_size_needed) {
	size_t needed = align_upto(stack_size_needed + 16, 16);
	
	fprintf(file, "\tsub sp, sp, #%zu\n", needed);
	fprintf(file, "\tstp fp, lr, [sp, #%zu]\n", needed - 16);
	fprintf(file, "\tadd fp, fp, #%zu\n", needed - 16);
	return;
}

static void function_after(FILE *file, size_t stack_size_needed) {
	size_t needed = align_upto(stack_size_needed + 16, 16);
	
	fprintf(file, "\tldp fp, lr, [sp, #%zu]\n", needed - 16);
	fprintf(file, "\tadd sp, sp, #%zu\n", needed);
	return;
}

enum { size32 = 0, size64 = 1 };

const char reg_size[2] = {
	[size32] = 'w', // 32-bit
	[size64] = 'x'  // 64-bit
};

static void load_variable(FILE *file, size_t stack_offset, int reg, int size) {
	fprintf(file, "\tldr %c%d, [fp, #%zu]\n", reg_size[size], reg, stack_offset);
	return;
}

static void store_variable(FILE *file, size_t stack_offset, int reg, int size) {
	fprintf(file, "\tstr %c%d, [fp, #%zu]\n", reg_size[size], reg, stack_offset);
	return;
}

/* var1 = var2; */
static void assign_variable(FILE *file, size_t var1, size_t var2, int size) {
	load_variable(file, var2, 9, size);
	store_variable(file, var1, 9, size);
	return;
}

static void load_imm(FILE *file, int reg, int size, uint64_t imm_) {
	uint64_t imm = imm_;
	int64_t imms = (int64_t)imm;

	if(imms <= 4095 && imms >= -4095) {
		fprintf(file, "\tmov %c%d, #%"PRId64"\n", reg_size[size], reg, imms);
		return;
	}

	if(imm <= 65535) {
		fprintf(file, "\tmovz %c%d, #%"PRIu64"\n", reg_size[size], reg, imm);
		return;
	}

	for(int i = 0; i < 4; i++) {
		fprintf(file, "\tmovk %c%d, #%"PRIu64", lsl #%d\n", reg_size[size], reg, imm & 0xffff, i * 16);
		imm >>= 16;
		if(!imm) break;
	}
	
	return;
}

/* var = imm; */
static void assign_variable_imm(FILE *file, size_t var, uint64_t imm, int size) {
	load_imm(file, 9, size, imm);
	store_variable(file, var, 9, size);
	return;
}


static uint64_t read_number(const char *s) {
	uint64_t number = 0;
	int negative = 0;

	int len = strlen(s);
	int i = 0;
	if(len == 0) return 0;


	/* check for negative (-) */
	if(len >= 2 && s[0] == '-') {
		negative = 1;
	}
	
	
	for(; i < len; i++) {
		int digit = s[i];
		if(digit >= '0' && digit <= '9') {
			number = (number * 10) + (digit - '0');
		}
	}

	if(negative) {
		number = (uint64_t)(-((int64_t)number));
	}

	return number;
}

FILE *generate_asm_arm64(LIST(node_base_t) node) {
	LOG(PRN_YLW, "called generate_asm(): arm64 backend");
	FILE* file = fopen("out.asm", "w");
	LOG(PRN_YLW, "opened file");
	fprintf(file, ".text\n"
				  ".globl _main\n"
				  ".p2align 2\n"
				  "_main:\n");
	function_prelude(file, stack_size);
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
		uint64_t imm = read_number(node.value[*i].statement_node->return_node->expr_node->int_lit_node->token.value);
		load_imm(*file, 0, size32, imm);
	}
	else if(node.value[*i].statement_node->return_node->expr_node->type == node_bin_expr) {
		LOG(PRN_YLW, "expr is bin_expr");
		generate_bin_expr(*node.value[*i].statement_node->return_node->expr_node, file, i, true);
		fprintf(*file, "\tmov x0, x8\n");
	}
	else {
		LOG(PRN_YLW, "expr is var");
		load_variable(*file, node.value[*i].statement_node->return_node->expr_node->var_node.stack_offset, 0, size32);
	}

	function_after(*file, stack_size);
	fprintf(*file, "\tret\n");
	LOG(PRN_YLW, "syscalled");
}

void generate_var_decl(LIST(node_base_t) node, FILE **file, size_t *i) {
	LOG(PRN_YLW, "called generate_var_decl()");
	if(node.value[*i].statement_node->var_decl_node->expr_node->type == node_int_lit) {
		LOG(PRN_YLW, "expr is int_lit");

		size_t var = node.value[*i].statement_node->var_decl_node->stack_offset;
		uint64_t num = read_number(node.value[*i].statement_node->var_decl_node->expr_node->int_lit_node->token.value);
		assign_variable_imm(*file, var, num, size32);
	}
	else if(node.value[*i].statement_node->var_decl_node->expr_node->type == node_var) {
		LOG(PRN_YLW, "expr is var");
		size_t var1 = node.value[*i].statement_node->var_decl_node->expr_node->var_node.stack_offset;
		size_t var2 = node.value[*i].statement_node->var_decl_node->stack_offset;
		assign_variable(*file, var1, var2, 0);
	}
	else if(node.value[*i].statement_node->var_decl_node->expr_node->type == node_bin_expr) {
		LOG(PRN_YLW, "expr is bin_expr");
		generate_bin_expr(*node.value[*i].statement_node->var_decl_node->expr_node, file, i, true);

		store_variable(*file, node.value[*i].statement_node->var_decl_node->stack_offset, 8, 0);
	}
	else {
		printf("\ncodegen error var decl\n");
		exit(1);
	}
}

void generate_label(LIST(node_base_t) node, FILE **file, size_t *i) {
	LOG(PRN_YLW, "called generate_label()");
	fprintf(*file, ".label_%s:\n", node.value[*i].statement_node->label_node->token.value);
}

void generate_goto(LIST(node_base_t) node, FILE **file, size_t *i) {
	LOG(PRN_YLW, "called generate_goto()");
	fprintf(*file, "\tb .label_%s\n", node.value[*i].statement_node->goto_node->token.value);
}

void generate_assignment(LIST(node_base_t) node, FILE **file, size_t *i) {
	LOG(PRN_YLW, "called generate_assignment()");
	if(node.value[*i].statement_node->assignment_node->rhs->type == node_var) {
		LOG(PRN_YLW, "rhs is var");

		size_t var1 = node.value[*i].statement_node->assignment_node->lhs.stack_offset;
		size_t var2 = node.value[*i].statement_node->assignment_node->rhs->var_node.stack_offset;

		assign_variable(*file, var1, var2, 0);
		
	}
	else if(node.value[*i].statement_node->assignment_node->rhs->type == node_int_lit) {
		LOG(PRN_YLW, "rhs is int_lit");

		size_t var = node.value[*i].statement_node->assignment_node->lhs.stack_offset;
		uint64_t num = read_number(node.value[*i].statement_node->assignment_node->rhs->int_lit_node->token.value);

		assign_variable_imm(*file, var, num, size32);
	}
	else if(node.value[*i].statement_node->assignment_node->rhs->type == node_bin_expr) {
		LOG(PRN_YLW, "rhs is bin_expr");
		generate_bin_expr(*node.value[*i].statement_node->assignment_node->rhs, file, i, true);
		store_variable(*file, node.value[*i].statement_node->assignment_node->lhs.stack_offset, 8, size32);
	}
	else {
		printf("\ncodegen error assignment\n");
		exit(1);
	}
}

void generate_bin_expr(node_expr_t expr, FILE **file, size_t *i, bool is_start) {
	LOG(PRN_YLW, "called generate_bin_expr()");
	switch(expr.bin_expr_node->op) {
	case op_add:
		LOG(PRN_YLW, "op is add");
		if(is_start) {
			if(expr.bin_expr_node->lhs->type == node_int_lit) {
				LOG(PRN_YLW, "lhs is int_lit");

				load_imm(*file, 8, size32, read_number(expr.bin_expr_node->lhs->int_lit_node->token.value));				
			}
			else if(expr.bin_expr_node->lhs->type == node_var) {
				LOG(PRN_YLW, "lhs is var");
				load_variable(*file, expr.bin_expr_node->lhs->var_node.stack_offset, 8, size32);
			}
			else {
				LOG(PRN_YLW, "ERROR");
				exit(1);
			}
		}
		if(expr.bin_expr_node->rhs->type == node_int_lit) {
			LOG(PRN_YLW, "rhs is int_lit");
			load_imm(*file, 10, size32, read_number(expr.bin_expr_node->rhs->int_lit_node->token.value));
			fprintf(*file, "\tadd w8, w8, w10\n");
		}
		else if(expr.bin_expr_node->rhs->type == node_var) {
			LOG(PRN_YLW, "rhs is var");
			load_variable(*file, expr.bin_expr_node->rhs->var_node.stack_offset, 10, size32);
			fprintf(*file, "\tadd w8, w8, w10\n");
		}
		else if(expr.bin_expr_node->rhs->type == node_bin_expr) {
			LOG(PRN_YLW, "rhs is bin_expr");
			if(expr.bin_expr_node->rhs->bin_expr_node->lhs->type == node_int_lit) {
				load_imm(*file, 10, size32, read_number(expr.bin_expr_node->rhs->bin_expr_node->lhs->int_lit_node->token.value));
				fprintf(*file, "\tadd w8, w8, w10\n");
			}
			else if(expr.bin_expr_node->rhs->bin_expr_node->lhs->type == node_var) {
				load_variable(*file, expr.bin_expr_node->rhs->bin_expr_node->lhs->var_node.stack_offset, 10, size32);
				fprintf(*file, "\tadd w8, w8, w10\n");
			}
			generate_bin_expr(*expr.bin_expr_node->rhs, file, i, false);
		}
		else {
			LOG(PRN_YLW, "ERROR");
			exit(1);
		}

		break;
	case op_sub:
		LOG(PRN_YLW, "op is sub");
		if(is_start) {
			if(expr.bin_expr_node->lhs->type == node_int_lit) {
				LOG(PRN_YLW, "lhs is int_lit");

				load_imm(*file, 8, size32, read_number(expr.bin_expr_node->lhs->int_lit_node->token.value));				
			}
			else if(expr.bin_expr_node->lhs->type == node_var) {
				LOG(PRN_YLW, "lhs is var");
				load_variable(*file, expr.bin_expr_node->lhs->var_node.stack_offset, 8, size32);
			}
			else {
				LOG(PRN_YLW, "ERROR");
				exit(1);
			}
		}
		if(expr.bin_expr_node->rhs->type == node_int_lit) {
			LOG(PRN_YLW, "rhs is int_lit");
			load_imm(*file, 10, size32, read_number(expr.bin_expr_node->rhs->int_lit_node->token.value));
			fprintf(*file, "\tsub w8, w8, w10\n");
		}
		else if(expr.bin_expr_node->rhs->type == node_var) {
			LOG(PRN_YLW, "rhs is var");
			load_variable(*file, expr.bin_expr_node->rhs->var_node.stack_offset, 10, size32);
			fprintf(*file, "\tsub w8, w8, w10\n");
		}
		else if(expr.bin_expr_node->rhs->type == node_bin_expr) {
			LOG(PRN_YLW, "rhs is bin_expr");
			if(expr.bin_expr_node->rhs->bin_expr_node->lhs->type == node_int_lit) {
				load_imm(*file, 10, size32, read_number(expr.bin_expr_node->rhs->bin_expr_node->lhs->int_lit_node->token.value));
				fprintf(*file, "\tsub w8, w8, w10\n");
			}
			else if(expr.bin_expr_node->rhs->bin_expr_node->lhs->type == node_var) {
				load_variable(*file, expr.bin_expr_node->rhs->bin_expr_node->lhs->var_node.stack_offset, 10, size32);
				fprintf(*file, "\tsub w8, w8, w10\n");
			}
			generate_bin_expr(*expr.bin_expr_node->rhs, file, i, false);
		}
		else {
			LOG(PRN_YLW, "ERROR");
			exit(1);
		}
		break;
	default:
		printf("\ngenerate_bin_expr\n");
		exit(1);
	}
}
