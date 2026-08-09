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
	node_compound_statement,
	node_label,
	node_goto,
	node_prim_expr,
	node_post_expr,
	node_mul_expr,
	node_add_expr,
	node_equal_expr,
	node_assign_expr,
	node_if,
	node_if_else,
	node_while,
	node_do_while,
	node_for,
	node_decl_for,
	node_fn,
	node_fn_decl,
	node_fn_call,
} node_type;

typedef enum op_type : u8 {
	op_add = 0,
	op_sub,
	op_mul,
	op_div,
	op_mod,
	op_equ,
	op_neq,
	op_fun,
} op_type;

typedef struct node_post_expr node_post_expr_t;
typedef struct node_mul_expr node_mul_expr_t;
typedef struct node_add_expr node_add_expr_t;
typedef struct node_equal_expr node_equal_expr_t;
typedef struct node_assign_expr node_assign_expr_t;
typedef struct node_compound_statement node_compound_statement_t;

// functions and variables
typedef struct obj {
	bool is_fn;
	token_t token;
	size_t stack_offset;
	node_compound_statement_t *body;
} obj_t;

typedef struct node_int_lit {
	token_t token;
} node_int_lit_t;

typedef struct node_prim_expr {
	node_type type;
	union {
		node_int_lit_t *int_lit_node;
		obj_t obj;
	};
} node_prim_expr_t;

typedef struct node_post_expr {
	node_type type;
	union {
		node_prim_expr_t *prim_expr_node;
		struct {
			op_type op;
			token_t token;
			node_post_expr_t *post_expr_node;
		};
	};
} node_post_expr_t;

typedef struct node_mul_expr {
	node_type type;
	union {
		node_post_expr_t *post_expr_node;
		struct {
			op_type op;
			node_mul_expr_t *lhs;
			node_post_expr_t *rhs;
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

typedef struct node_equal_expr {
	node_type type;
	union {
		node_add_expr_t *add_expr_node;
		struct {
			op_type op;
			node_equal_expr_t *lhs;
			node_add_expr_t *rhs;
		};
	};
} node_equal_expr_t;

typedef struct node_assign_expr {
	node_type type;
	union {
		node_equal_expr_t *equal_expr_node;
		struct {
			obj_t lhs;
			node_equal_expr_t *rhs;
		};
	};
} node_assign_expr_t;

typedef struct node_expr {
	node_assign_expr_t *assign_expr_node;
} node_expr_t;

typedef struct node_var_decl {
	token_t token;
	size_t stack_offset;
	node_expr_t *expr_node;
} node_var_decl_t;

typedef struct node_fn_decl {
	token_t token;
	node_compound_statement_t *body;
} node_fn_decl_t;

typedef struct node_return {
	node_expr_t *expr_node;
} node_return_t;

typedef struct node_label {
	token_t token;
} node_label_t;

typedef struct node_goto {
	token_t token;
} node_goto_t;

typedef struct node_statement node_statement_t;
typedef struct node_if {
	node_type type;
	node_expr_t *expr_node;
	node_statement_t *if_branch;
	node_statement_t *else_branch;
} node_if_t;

typedef struct node_while {
	node_type type;
	node_expr_t *expr_node;
	node_statement_t *body;
} node_while_t;

typedef struct node_for {
	node_type type;
	node_var_decl_t *var_decl_node;
	node_expr_t *expr1;
	node_expr_t *expr2;
	node_expr_t *expr3;
	node_statement_t *body;
} node_for_t;

typedef struct node_compound_statement node_compound_statement_t;
typedef struct node_statement {
	node_type type;
	union {
		node_var_decl_t *var_decl_node;
		node_return_t *return_node;
		node_label_t *label_node;
		node_goto_t *goto_node;
		node_compound_statement_t *compound_statement_node;
		node_if_t *if_node;
		node_while_t *while_node;
		node_for_t *for_node;
		node_expr_t *expr_node;
	};
} node_statement_t;

typedef node_statement_t* node_stmt_ptr;
NEW_LIST(node_stmt_ptr);
typedef struct node_compound_statement {
	LIST(node_stmt_ptr) statement_nodes;
} node_compound_statement_t;

typedef struct node_base {
	node_fn_decl_t *fn_decl_node;
} node_base_t;

NEW_LIST(obj_t);
typedef struct scope {
	LIST(obj_t) objs;
} scope_t;

NEW_LIST(scope_t);
NEW_LIST(node_base_t);

extern size_t stack_size;
extern LIST(scope_t) scopes;

LIST(node_base_t) parse(LIST(token_t) tokens);
node_int_lit_t *parse_int_lit(LIST(token_t) tokens, size_t *i);
node_return_t *parse_return(LIST(token_t) tokens, size_t *i);
obj_t parse_var(LIST(token_t) tokens, size_t *i);
node_expr_t *parse_expr(LIST(token_t) tokens, size_t *i);
node_post_expr_t *parse_post_expr(LIST(token_t) tokens, size_t *i);
node_mul_expr_t *parse_mul_expr(LIST(token_t) tokens, size_t *i);
node_add_expr_t *parse_add_expr(LIST(token_t) tokens, size_t *i);
node_equal_expr_t *parse_equal_expr(LIST(token_t) tokens, size_t *i);
node_var_decl_t *parse_var_decl(LIST(token_t) tokens, size_t *i);
node_label_t *parse_label(LIST(token_t) tokens, size_t *i);
node_goto_t *parse_goto(LIST(token_t) tokens, size_t *i);
node_assign_expr_t *parse_assign_expr(LIST(token_t) tokens, size_t *i);
node_compound_statement_t *parse_compound_statement(LIST(token_t) tokens, size_t *i);
node_if_t *parse_if(LIST(token_t) tokens, size_t *i);
node_while_t *parse_while(LIST(token_t) tokens, size_t *i);
node_fn_decl_t *parse_fn_decl(LIST(token_t) tokens, size_t *i);
node_statement_t *parse_statement(LIST(token_t) tokens, size_t *i);

bool identifier_is_var(token_t token);
bool identifier_is_fn(token_t token);

#endif // PARSER_H
