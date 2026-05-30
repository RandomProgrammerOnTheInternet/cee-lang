#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include "ds.h"
#include "util.h"
#include "lexer.h"

typedef enum node_type : u8 {
	node_base = 0,
	node_return,
	node_int_lit,
	node_var,
	node_var_decl,
	node_expr,
	node_statement,
	node_label,
	node_goto,
	node_assignment,
	node_prim_expr,
	node_mul_expr,
	node_add_expr
} node_type;

typedef enum op_type : u8 {
	op_add = 0,
	op_sub,
	op_mul,
	op_div,
	op_mod
} op_type;

typedef struct node_mul_expr node_mul_expr_t;
typedef struct node_add_expr node_add_expr_t;

typedef struct node_int_lit {
	token_t token;
} node_int_lit_t;

typedef struct node_var {
	size_t stack_offset;
	token_t token;
} node_var_t;

typedef struct node_prim_expr {
	node_type type;
	union {
		node_int_lit_t *int_lit_node;
		node_var_t var_node;
	};
} node_prim_expr_t;

typedef struct node_mul_expr {
	node_type type;
	union {
		node_prim_expr_t *prim_expr_node;
		struct {
			op_type op;
			node_mul_expr_t *lhs;
			node_prim_expr_t *rhs;
		};
	};
} node_mul_expr_t;

typedef struct node_add_expr {
	node_type type;
	union {
		node_mul_expr_t *mul_expr_node;
		struct {
			op_type op;
			node_add_expr_t *lhs;
			node_mul_expr_t *rhs;
		};
	};
} node_add_expr_t;


typedef struct node_expr {
	node_add_expr_t *add_expr_node;
} node_expr_t;

typedef struct node_var_decl {
	token_t token;
	size_t stack_offset;
	node_expr_t *expr_node;
} node_var_decl_t;

typedef struct node_assignment {
	node_var_t lhs;
	node_expr_t *rhs;
} node_assignment_t;

typedef struct node_return {
	node_expr_t *expr_node;
} node_return_t;

typedef struct node_label {
	token_t token;
} node_label_t;

typedef struct node_goto {
	token_t token;
} node_goto_t;

typedef struct node_statement {
	node_type type;
	union {
		node_var_decl_t *var_decl_node;
		node_return_t *return_node;
		node_label_t *label_node;
		node_goto_t *goto_node;
		node_assignment_t *assignment_node;
	};
} node_statement_t;

typedef struct node_base {
	node_type type;
	union {
		node_statement_t *statement_node;
		node_expr_t *expr_node;
	};
} node_base_t;

NEW_LIST(node_var_t);
NEW_LIST(node_base_t);

extern size_t stack_size;
extern LIST(node_var_t) variable_lookup;

LIST(node_base_t) parse(LIST(token_t) tokens);
node_int_lit_t *parse_int_lit(LIST(token_t) tokens, size_t *i);
node_return_t *parse_return(LIST(token_t) tokens, size_t *i);
node_var_t parse_var(LIST(token_t) tokens, size_t *i);
node_expr_t *parse_expr(LIST(token_t) tokens, size_t *i);
node_mul_expr_t *parse_mul_expr(LIST(token_t) tokens, size_t *i);
node_mul_expr_t *parse_mul_expr_rec(LIST(token_t) tokens, size_t *i, node_mul_expr_t *mul_expr);
node_add_expr_t *parse_add_expr(LIST(token_t) tokens, size_t *i);
node_add_expr_t *parse_add_expr_rec(LIST(token_t) tokens, size_t *i, node_add_expr_t *add_expr);
node_var_decl_t *parse_var_decl(LIST(token_t) tokens, size_t *i);
node_label_t *parse_label(LIST(token_t) tokens, size_t *i);
node_goto_t *parse_goto(LIST(token_t) tokens, size_t *i);
node_assignment_t *parse_assignment(LIST(token_t) tokens, size_t *i);
node_statement_t *parse_statement(LIST(token_t) tokens, size_t *i);

bool identifier_is_var(token_t token);

#endif // PARSER_H
