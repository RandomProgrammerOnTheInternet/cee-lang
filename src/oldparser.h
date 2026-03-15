#ifndef PARSER_H
#define PARSER_H

#include "ds.h"
#include "util.h"

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

typedef enum bin_expr_type : u8 {
	bin_expr_add = 0
} bin_expr_type;

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
	enum node_type type;
	union {
		node_int_lit_t *int_lit_node;
		node_var_t *var_node;
		node_bin_expr_t *bin_expr_node;
	};
} node_expr_t;

struct node_bin_expr_t {
	bin_expr_type op;
	node_expr_t *lhs;
	node_expr_t *rhs;
};

typedef struct node_var_decl_t {
	token_t token;
	size_t stack_offset;
	node_expr_t *expr_node;
} node_var_decl_t;

typedef struct node_assignment_t {
	node_var_t *lhs;
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

size_t stack_size = 0;
LIST(node_var_t) variable_lookup;

LIST(node_base_t) parse(LIST(token_t) tokens);
node_int_lit_t *parse_int_lit(LIST(token_t) tokens, size_t *i);
node_return_t *parse_return(LIST(token_t) tokens, size_t *i);
node_var_t *parse_var(LIST(token_t) tokens, size_t *i);
node_expr_t *parse_expr(LIST(token_t) tokens, size_t *i);
node_bin_expr_t *parse_bin_expr(LIST(token_t) tokens, size_t *i);
node_var_decl_t *parse_var_decl(LIST(token_t) tokens, size_t *i);
node_label_t *parse_label(LIST(token_t) tokens, size_t *i);
node_goto_t *parse_goto(LIST(token_t) tokens, size_t *i);
node_assignment_t *parse_assignment(LIST(token_t) tokens, size_t *i);
node_statement_t *parse_statement(LIST(token_t) tokens, size_t *i);

bool identifier_is_var(token_t token);

#endif // PARSER_H

#ifdef PARSER_IMPL

LIST(node_base_t) parse(LIST(token_t) tokens) {
	LOG(PRN_GRN, "called parse()");
	INIT_LIST(variable_lookup, 0);
	// trust me on this
	LIST(node_base_t) base_node;
	INIT_LIST(base_node, 0);
	for(size_t i = 0; i < tokens.length; i++) {
		LOG(PRN_GRN, "loop");
		switch(tokens.value[i].type) {
		case token_keyword_goto: [[fallthrough]];
		case token_keyword_int: [[fallthrough]];
		case token_keyword_return: [[fallthrough]];
		case token_identifier:
			LOG(PRN_GRN, "detected statement");
			LIST_APPEND(base_node, ((node_base_t) {
				.type = node_statement,
				.statement_node = parse_statement(tokens, &i)
			}));
			LOG(PRN_GRN, "added statement");
			break;
		case token_op_semicolon: break;
		default:
			break;
		}
	}
	
	return base_node;
}

node_int_lit_t *parse_int_lit(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "called parse_int_lit()");
	if(*i >= tokens.length) {
		error(error_missing_expression, "your missing a number after the return");
	}
	if(tokens.value[*i].type != token_int_literal) {
		error(error_invalid_expression, "there is supposed to be a number after the return");
	}
	node_int_lit_t *node = malloc(sizeof(node_int_lit_t));
	LOG(PRN_GRN, "memory allocated");
	*node = (node_int_lit_t){
		.token = tokens.value[*i]
	};
	
	return node;
}

node_return_t *parse_return(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "called parse_return()");
	++*i;
	node_return_t *node = malloc(sizeof(node_return_t));
	LOG(PRN_GRN, "memory allocated");
	*node = (node_return_t) {
		.expr_node = parse_expr(tokens, i)
	};
	++*i;
	if(tokens.value[*i].type != token_op_semicolon) {
		error(error_missing_semicolon, "bro you missed a semicolon at the end of the return");
	}
	
	return node;
}

node_var_t *parse_var(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "called parse_var()");
	size_t j;
	LOG(PRN_GRN, "variable_lookup.length = %zu", variable_lookup.length);
	for(j = 0; j < variable_lookup.length; j++) {
		LOG(PRN_GRN, "loop: %s", variable_lookup.value[j].token.value);
		if(strcmp(tokens.value[*i].value,
				  variable_lookup.value[j].token.value) == 0) {
			LOG(PRN_GRN, "var parsed");
			return &variable_lookup.value[j];
		}
	}
	printf("uh oh!!\n");
	exit(1);
}

node_expr_t *parse_expr(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "called parse_expr()");
	node_expr_t *node = malloc(sizeof(node_expr_t));
	LOG(PRN_GRN, "memory allocated");
	enum token_type op = tokens.value[*i + 1].type;
	switch(tokens.value[*i].type) {
	case token_int_literal:
		LOG(PRN_GRN, "int_lit");
		if(op == token_op_plus) {
			LOG(PRN_GRN, "bin_expr");
			*node = (node_expr_t) {
				.type = node_bin_expr,
				.bin_expr_node = parse_bin_expr(tokens, i)
			};
		}
		LOG(PRN_GRN, "not bin_expr");
		*node = (node_expr_t) {
			.type = node_int_lit,
			.int_lit_node = parse_int_lit(tokens, i)
		};
		return node;
		break;
	case token_identifier:
		LOG(PRN_GRN, "identifier");
		if(op == token_op_plus) {
			LOG(PRN_GRN, "bin_expr");
			*node = (node_expr_t) {
				.type = node_bin_expr,
				.bin_expr_node = parse_bin_expr(tokens, i)
			};
			return node;
			break;
		}
		LOG(PRN_GRN, "not bin_expr");
		*node = (node_expr_t) {
			.type = node_var,
			.var_node = parse_var(tokens, i)
		};
		return node;
		break;
	default:
		printf("\nerror invalid expr\n");
		exit(1);
	}
	printf("uh oh!\n");
	exit(1);
}

node_bin_expr_t *parse_bin_expr(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "%d", bin_expr_add);
	LOG(PRN_GRN, "called parse_bin_expr()");
	node_bin_expr_t *node = malloc(sizeof(node_bin_expr_t));
	LOG(PRN_GRN, "memory allocated");
	if(tokens.value[*i].type == token_int_literal) {
		LOG(PRN_GRN, "lhs is int_lit");
		node_expr_t *expr = malloc(sizeof(node_expr_t));
		expr->int_lit_node = parse_int_lit(tokens, i);
		expr->type = node_int_lit;
		node->lhs = expr;
	}
	else if(tokens.value[*i].type == token_identifier) {
		LOG(PRN_GRN, "lhs is identifier");
		node_expr_t *expr = malloc(sizeof(node_expr_t));
		expr->var_node = parse_var(tokens, i);
		expr->type = node_var;
		node->lhs = expr;
	}
	++*i;
	switch(tokens.value[*i].type) {
	case token_op_plus:
		LOG(PRN_GRN, "op is plus");
		node->op = bin_expr_add;
		LOG(PRN_GRN, "op: %d", node->op);
		break;
	default:
		printf("\nerror invalid op binexpr\n");
		exit(1);
	}
	++*i;
	node->rhs = parse_expr(tokens, i);
	LOG(PRN_GRN, "rhs set");

	return node;
}

node_var_decl_t *parse_var_decl(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "called parse_var_decl()");
	if(tokens.value[*i].type != token_keyword_int) {
		printf("\nerror invalid type\n");
		exit(1);
	}
	++*i;
	if(tokens.value[*i].type != token_identifier) {
		printf("\nerror no identifier\n");
		exit(1);
	}
	token_t ident = tokens.value[*i];
	++*i;
	if(tokens.value[*i].type != token_op_equals) {
		printf("\nerror no equals\n");
		exit(1);
	}
	++*i;
	if(tokens.value[*i].type != token_int_literal && tokens.value[*i].type != token_identifier) {
		printf("\nerror no int lit\n");
		exit(1);
	}
	node_expr_t *expr = parse_expr(tokens, i);
	++*i;
	if(tokens.value[*i].type != token_op_semicolon) {
		printf("\nerror no semicolon\n");
		exit(1);
	}
	stack_size += sizeof(int);
	LOG(PRN_GRN, "stack_size incremented");
	node_var_t *var_node = malloc(sizeof(node_var_t));
	LOG(PRN_GRN, "memory allocated");
	*var_node = (node_var_t) {
		.stack_offset = stack_size,
		.token = ident
	};
	LOG(PRN_GRN, "stack offset set");
	LOG(PRN_GRN, "token set");
	LIST_APPEND(variable_lookup, *var_node);
	node_var_decl_t *decl_node = malloc(sizeof(node_var_decl_t));
	LOG(PRN_GRN, "memory allocated");
	*decl_node = (node_var_decl_t) {
		.token = ident,
		.expr_node = expr,
		.stack_offset = stack_size
	};
	LOG(PRN_GRN, "token set");
	LOG(PRN_GRN, "expr set");
	LOG(PRN_GRN, "stack offset set");
	return decl_node;
}

node_label_t *parse_label(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "called parse_label()");
	++*i;
	node_label_t *node = malloc(sizeof(node_label_t));
	LOG(PRN_GRN, "memory allocated");
	*node = (node_label_t) {
		.token = tokens.value[*i - 1]
	};
	LOG(PRN_GRN, "token set");
	return node;
}

node_goto_t *parse_goto(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "called parse_goto()");
	++*i;
	if(tokens.value[*i].type != token_identifier) {
		printf("error no identifier in goto\n");
		exit(1);
	}
	++*i;
	if(tokens.value[*i].type != token_op_semicolon) {
		printf("error missing semicolon in goto\n");
		exit(1);
	}
	node_goto_t *node = malloc(sizeof(node_goto_t));
	LOG(PRN_GRN, "memory allocated");
	*node = (node_goto_t) {
		.token = tokens.value[*i - 1]
	};
	LOG(PRN_GRN, "token set");
	return node;
}

node_assignment_t *parse_assignment(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "called parse_assignment()");
	node_assignment_t *node = malloc(sizeof(node_assignment_t));
	LOG(PRN_GRN, "memory allocated");
	node->lhs = parse_var(tokens, i);
	LOG(PRN_GRN, "set lhs");
	++*i;
	if(tokens.value[*i].type != token_op_equals) {
		printf("missing equals in assignmen\n");
		exit(1);
	}
	++*i;
	node->rhs = parse_expr(tokens, i);
	LOG(PRN_GRN, "rhs set");
	return node;
}

node_statement_t *parse_statement(LIST(token_t) tokens, size_t *i) {
	LOG(PRN_GRN, "called parse_statement()");
	node_statement_t *node = malloc(sizeof(node_statement_t));
	LOG(PRN_GRN, "memory allocated");
	switch(tokens.value[*i].type) {
	case token_keyword_return:
		LOG(PRN_GRN, "detected keyword return");
		*node = (node_statement_t) {
			.type = node_return,
			.return_node = parse_return(tokens, i)
		};
		LOG(PRN_GRN, "end return");
		return node;
		break;
	case token_keyword_goto:
		LOG(PRN_GRN, "detected keyword goto");
		*node = (node_statement_t) {
			.type = node_goto,
			.goto_node = parse_goto(tokens, i)
		};
		LOG(PRN_GRN, "end goto");
		return node;
		break;
	case token_keyword_int:
		LOG(PRN_GRN, "detected keyword int");
		*node = (node_statement_t) {
			.type = node_var_decl,
			.var_decl_node = parse_var_decl(tokens, i)
		};
		LOG(PRN_GRN, "end int");
		return node;
		break;
	case token_identifier:
		LOG(PRN_GRN, "token_identifier: %s", tokens.value[*i].value);
		if(identifier_is_var(tokens.value[*i])) {
			LOG(PRN_GRN, "identifier is var");
			*node = (node_statement_t) {
				.type = node_assignment,
				.assignment_node = parse_assignment(tokens, i)
			};
			return node;
		}
		else if(tokens.value[*i + 1].type == token_op_colon) {
			LOG(PRN_GRN, "identifier is label");
			*node = (node_statement_t) {
				.type = node_label,
				.label_node = parse_label(tokens, i)
			};
			return node;
		}
		else {
			printf("\nerror invalid identifier\n");
			exit(1);
		}
		break;
	default:
		printf("\nerror invalid parse_statement\n");
		exit(1);
	}
	printf("impossible error parse_statement\n");
	exit(1);
}

bool identifier_is_var(token_t token) {
	LOG(PRN_GRN, "called identifier_is_var()");
	for(size_t i = 0; i < variable_lookup.length; i++) {
		if(!strcmp(token.value, variable_lookup.value[i].token.value)) {
			LOG(PRN_GRN, "identifier is var");
			return true;
		}
	}
	LOG(PRN_GRN, "identifier is not var");
	return false;
}

#endif // PARSER_IMPL
