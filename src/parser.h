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
	node_bin_expr
} node_type;

typedef enum op_type : u8 {
	op_add = 0,
	op_sub,
	op_mul,
	op_div,
	op_mod
} op_type;

static const u8 op_precedence[] = {
	2, // op_add
	2, // op_sub
	1, // op_mul
	1, // op_div
	1  // op_mod
};

typedef struct node_int_lit_t {
	token_t token;
} node_int_lit_t;

typedef struct node_var_t {
	size_t stack_offset;
	token_t token;
} node_var_t;

struct node_bin_expr_t;
typedef struct node_bin_expr_t node_bin_expr_t;

typedef struct node_expr_t {
	node_type type;
	union {
		node_int_lit_t *int_lit_node;
		node_var_t var_node;
		node_bin_expr_t *bin_expr_node;
	};
} node_expr_t;

struct node_bin_expr_t {
	op_type op;
	node_expr_t *lhs;
	node_expr_t *rhs;
};

typedef struct node_var_decl_t {
	token_t token;
	size_t stack_offset;
	node_expr_t *expr_node;
} node_var_decl_t;

typedef struct node_assignment_t {
	node_var_t lhs;
	node_expr_t *rhs;
} node_assignment_t;

typedef struct node_return_t {
	node_expr_t *expr_node;
} node_return_t;

typedef struct node_label_t {
	token_t token;
} node_label_t;

typedef struct node_goto_t {
	token_t token;
} node_goto_t;

typedef struct node_statement_t {
	node_type type;
	union {
		node_var_decl_t *var_decl_node;
		node_return_t *return_node;
		node_label_t *label_node;
		node_goto_t *goto_node;
		node_assignment_t *assignment_node;
	};
} node_statement_t;

typedef struct node_base_t {
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
node_bin_expr_t *parse_bin_expr(LIST(token_t) tokens, size_t *i);
node_var_decl_t *parse_var_decl(LIST(token_t) tokens, size_t *i);
node_label_t *parse_label(LIST(token_t) tokens, size_t *i);
node_goto_t *parse_goto(LIST(token_t) tokens, size_t *i);
node_assignment_t *parse_assignment(LIST(token_t) tokens, size_t *i);
node_statement_t *parse_statement(LIST(token_t) tokens, size_t *i);

bool identifier_is_var(token_t token);

#endif // PARSER_H
